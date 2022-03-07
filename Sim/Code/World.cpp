#include"World.h"



World::World()
{
}


Agent* World::CreateAgent(int id, float pos_x, float pos_y)
{
    std::shared_ptr<Agent> a = std::make_shared<Agent> (this,id, pos_x, pos_y);
    Crowd.insert(a);
    // Compute neighborhoods :
     a->ComputeNeighbors(); // Compute the Neighbors
    return a.get();
}

void World::SolveSimpleContact(Agent* agent)
{
    std::set<Agent*> neighbors = agent->getNeighbors();
bool contact_flag = false;
bool push_flag = false;
Push* current_push;
Vector2D pushing_force;

    for (Agent* n : neighbors)
    {


        // Check if the is a contact :
        // if there is contact and the agent is still moving toward the neighbor
        if(agent->CheckContact(n))
        {   
            
            pushing_force = 1e4*(agent->getRadius_a()+n->getRadius_a()-(agent->getPosition()-n->getPosition()).magnitude())
                            *(agent->getPosition()-n->getPosition()).getnormalized();
            agent->setExtForces(pushing_force);
        }
    }
    // agent->setDownAcceleration(agent->getAcceleration());
}

void World::ComputeContacts(Agent* agent)
{
    std::set<Agent*> neighbors = agent->getNeighbors();
    bool contact_flag = false;
    bool push_flag = false;
    Push* current_push;

    for (Agent* n : neighbors)
    {

        // Check if the is a contact :
        // if there is contact and the agent is still moving toward the neighbor
        if(agent->CheckContact(n))
        {   
            contact_flag = true;
            std::cout << "Contact with : "<< n->getID() <<std::endl;
            push_flag=false;
            // Check if a Push instance already exist
            //std::cout << "contact between "<< agent->getID()<<" and "<< n->getID() <<" dot : "<<(agent->getVelocity()).dot(n->getPosition() - agent->getPosition()) <<std::endl;

            // Chech if the agent is pushing 
            for (Push* p : agent->getPushes())
            {
                if (p->getReceiver() == n)
                {
                    push_flag=true;
                    std::cout << "Pushing : " <<(p->getReceiver())->getID() << std::endl;
                }
            }
            // Check if the agent is beeing pushed
            //if(push_flag){std::cout << agent->getID() <<" push "<<n->getID() <<endl;}
            for (Push* p : n->getPushes())
            {
                if (p->getReceiver() == agent)
                {
                    push_flag=true;
                    std::cout << "Recieving push from : " <<(p->getPusher())->getID() << std::endl;
                }
            }
            // If the agent is going toward the neighbor and there isn't a Push instance created
            // The pusher is the one with the biggest momentum along the axis passing through the two agents
            if(!push_flag ) 
            {   
                Vector2D axis = agent->getPosition() - n->getPosition();
                if (fabs((agent->getMass()*agent->getVelocity()).dot(axis)) > fabs((n->getMass()*n->getVelocity()).dot(axis)) )
                {
                    std::cout << "Push creation : Pusher " << agent->getID() <<" Receiver : "<<n->getID() <<endl;
                    current_push = agent->CreatePush(n); 
                }
                else
                {
                     std::cout << "Push creation : Pusher " << n->getID() <<" Receiver : "<<agent->getID() <<endl;
                    current_push = n->CreatePush(agent);
                }
            } 
            std::cout << "push_flag statue: "<< push_flag << std::endl;
        }
    }
    

}

void World::SolveContacts(Agent* agent)
{
    Vector2D pushing_force;
    Vector2D acceleration;

    // For each pushes
    for (Push* p : agent->getPushes())
    {
        std::cout << "push clock: "<<p->getClock()<<std::endl;
        Agent* receiver=p->getReceiver();
        // If push.clock > Reaction time
        if(p->getClock() > AvgReactionTime)
        {
            std::cout << " - Active Contact"<< " receiver = "<< receiver->getID()<<std::endl;
            //ActiveContact(p);
            //SolveSimpleContact(p->getPusher());
            //SolveSimpleContact(p->getReceiver());

        }
        else 
        {   
            std::cout <<" - Passive Contact"<< " receiver = "<< receiver->getID()<< std::endl;
            PassiveContact(p);
        
        }
        
        // At the end : check if there is still contact
        if (agent->CheckContact(receiver)){p->ClockTick(TimeStep);} // yes : increment the clock
        else { agent->DeletePush(p); 
        std::cout <<"Push of "<< agent->getID()<< " on "<< receiver->getID()<<  " is deleted"<<std::endl;} // no : delete the push instance
    }

}

void World::ActiveContact(Push* push)  // Solve the active reaction of the pusher onto the receiver
{
    Agent* pusher = push->getPusher();
    Agent* receiver = push->getReceiver();
    //#### Stop the velocity of the CoM toward the Receiver
    if (pusher->getVelocity().dot(receiver->getPosition()-pusher->getPosition())>0 )
    {
        Vector2D VelToSup = pusher->getVelocity().dot((receiver->getPosition()-pusher->getPosition()).getnormalized())*(receiver->getPosition()-pusher->getPosition()).getnormalized();
        pusher->setVelocity(pusher->getVelocity() - VelToSup);
        std::cout << "- velocity after stop :"<< VelToSup.x <<std::endl;
        //pusher->setAcceleration( pusher->getAcceleration() + (pusher->getVelocity() - VelToSup)/TimeStep);
    }

    //#### Check if the agent can mooves its Down toward its CoM
    Vector2D Down_to_Up = pusher->getPosition() - pusher->getDownPosition();

    // Check if there is conflict with another Down
    Vector2D DownPos_at_t0 = pusher->getDownPosition(); // Backup the current Down position
    
    // The Down position if pseudo-updated :
    Vector2D DownAimVelocity = Down_to_Up.getnormalized()* std::min(pusher->getMaximumSpeed(),Down_to_Up.magnitude()/(TimeStep) ) ;
    pusher->setDownPosition(pusher->getDownPosition() + DownAimVelocity*TimeStep);
    
    
    bool conflict_flag =false;
    for (Agent* n : pusher->getNeighbors())
    {
            conflict_flag = pusher->CheckDownContact(n);
    }
    pusher->setDownPosition(DownPos_at_t0); // Position reset

    // #### Reaction based on the ability to move its Down
    // If conflict : the componante of the velocity in the direction of the receiver is stoped and the pusher apply a force on the receiver
    if (conflict_flag)
    {
        //Vector2D pushing_force = 2* pusher->getVelocity().magnitude()*(receiver->getPosition() - pusher->getPosition()).getnormalized();
        Vector2D pushing_force = 150*(exp(10*(0.45-push->getClock()))-0.5) *(receiver->getPosition()-pusher->getPosition()).getnormalized();
        
        //Vector2D pushing_force = 0.5*(pusher->getDownPosition()-pusher->getPosition());
        
        pusher->setExtForces( pusher->getExtForces()+ pushing_force);
        // During the reaction, the receiver get a part of the force apply by a1
        receiver->setExtForces(receiver->getExtForces() - k_power*pushing_force*(receiver->getMass()/receiver->getMass()));
        
        
    }
    else // If no conflict : the Down goes toward the CoM (which staies in place)
    {
        pusher->setDownAcceleration(pusher->getDownAcceleration() + (DownAimVelocity-pusher->getDownVelocity())/TimeStep );
    }

}

void World::PassiveContact(Push* push) // Solve the  passive reaction of the pusher onto the receiver
{
    Agent* pusher = push->getPusher();
    Agent* receiver = push->getReceiver();

    float Mp = pusher->getMass();
    float Mr = receiver->getMass();
    // If the agent haven't started to react but is still moving toward the receiver
    if (pusher->getVelocity().dot(receiver->getPosition()-pusher->getPosition()) >0)
    {
        // In this config the pusher hasn't started to react yet : v2 = k_momentum *||v1||e12*m1/m2
        Vector2D dir_vec = (receiver->getPosition()-pusher->getPosition()).getnormalized();
        receiver->setExtForces( receiver->getExtForces()+ ( (k_momentum*pusher->getVelocity().magnitude()*Mp*dir_vec 
                                - receiver->getVelocity()*Mr)/TimeStep ) );
        // the velocity is set by having : acceleration = (Vgoal - V0)/dt and Fext = acceleration*dt

        std::cout << "Receiver get passive contact force :"<< receiver->getExtForces().magnitude()<<" N" << std::endl;
        std::cout << "Direction : "<< dir_vec.x<<", " << dir_vec.y<< std::endl;

    }

}

void World::Iterate() 
{
// Computation loop
Vector2D NulVector = Vector2D();
for (std::shared_ptr<Agent> agent:Crowd)
    {

        std::cout << " #### Agent id :" << agent->getID() <<" ####"<<std::endl;

        
/*         // #### Motion policy : Here == if no contact and stability OK (meaning no push) then stop moving
        bool contact_flag = false;
        std::cout << "Nb of Neighbors : "<< (agent->getNeighbors()).size()<<std::endl;
        for (Agent* n : agent->getNeighbors())
        {
            // Check if the is a contact :
            
            contact_flag = agent->CheckContact(n);
            if(contact_flag){std::cout <<"Contact with :" <<  n->getID() <<std::endl; break;}
        } */
        

        // ####Initial push
        if (agent->getID() ==0)
        {
            Vector2D offset = (500)*Vector2D( 1.0 , 0.0 );
            if(Clock>=0.125 && Clock<=0.25)
            {
                agent->setExtForces(0.5*offset);
                std::cout <<"Applying half init push on Agent 0"<< std::endl;
            }
            if(Clock>0.25 && Clock<=0.5)
            {
                agent->setExtForces(offset);
                std::cout <<"Applying full init push on Agent 0"<< std::endl;
            } 
        }

        // #### Contact and stability solving ####
        ComputeContacts(agent.get());
        SolveContacts(agent.get());

        // ## Simple contact 
        //SolveSimpleContact(agent.get());

       

    }
// Set the size of the next TimeStep
// SetTimeStep(ComputeNextTimeStep( TimeStep, 0.1, 1.05, 0.3) );
std::cout << "#### Stability Solving ####" <<std::endl;
for(std::shared_ptr<Agent> agent:Crowd)
{
    std::cout << "## Agent "<< agent->getID() << "##" <<std::endl;
    std::cout << "CoM 0: Position : "<<agent->getPosition().x <<" velocity :"<<agent->getVelocity().x<< " Acceleration :"<<agent->getAcceleration().x<< std::endl; 
    std::cout << "Down 0: Position : "<<agent->getDownPosition().x <<" velocity :"<<agent->getDownVelocity().x<< " Acceleration :"<<agent->getDownAcceleration().x<< std::endl;
        // Update positions 
        agent->Update(TimeStep);

        // reset External forces
        agent->setExtForces( NulVector);

    // ##### Display end of the loop state
    std::cout << "CoM 1: Position :"<<agent->getPosition().x <<" velocity :"<<agent->getVelocity().x<< " Acceleration :"<<agent->getAcceleration().x<< std::endl; 
    std::cout << "Down 1: Position :"<<agent->getDownPosition().x <<" velocity :"<<agent->getDownVelocity().x<< " Acceleration :"<<agent->getDownAcceleration().x<< std::endl;
    std::cout << "\n";


}

// tick the clock with the chosen TimeStep
ClockTick(TimeStep);


}

// TOOLS :
std::string World::ListAgents() const
{
    std::ostringstream flux;

    for(std::shared_ptr<Agent> a:Crowd)
    {
        flux << a->ToString() <<std::endl;
    }
    return flux.str();
}


float World::ComputeNextTimeStep( float CurrentTimeStep, float SaftyCoef, float AmpliCoeff, float MaxTimeStep)
{
    float MinCFLTimeStep = CurrentTimeStep;
    float Vmax;
    for (std::shared_ptr<Agent> agent:Crowd)
    {
        Vmax= max(std::abs(agent->getVelocity().magnitude())+CurrentTimeStep*agent->getExtForces().magnitude(), std::abs(agent->getDownVelocity().magnitude()) );
        if (agent->getVelocity().magnitude() != 0.0)
        {
             MinCFLTimeStep = min(MinCFLTimeStep, SaftyCoef*agent->getRadius_a()/Vmax);
        }
    }
    // If the time step in unchanged after checking all the agent
    if (MinCFLTimeStep == CurrentTimeStep && MinCFLTimeStep < MaxTimeStep)
    {
        // Then the time step is raise using the Ampli coeff until it reach the MaxTimeStep
        MinCFLTimeStep = MinCFLTimeStep*AmpliCoeff;
    }

    return MinCFLTimeStep;
}

void World::ClockTick(float dt)
{
    Clock += dt;
}