#include "Agent.h"


Agent::Agent(World* w,int id, float pos_x, float pos_y) : world(w), id_(id),position_(pos_x,pos_y),DownPosition_(pos_x,pos_y) // Constructeur par défaut
{
    Radius_a=world->getAvgAgentRadius_a();
    Radius_b=world->getAvgAgentRadius_b();
    direction_=Vector2D(1,0);
    setMass(world->getAvgMass());
}

std::string Agent::ToString() const
{
    std::ostringstream flux;
    flux << id_ <<'\t'<< position_.x << '\t'<< position_.y << '\t' <<DownPosition_.x << '\t'<< DownPosition_.y <<std::endl;
    return flux.str();
}


void Agent::setNeighbors(Agent* NeighborAgent)
{
    if (neighbors_.find(NeighborAgent) == neighbors_.end() && NeighborAgent!=this )
    {
        neighbors_.insert(NeighborAgent);    // Create a pointer on the Neighboring Agent and ad it to the set _neighbors
        NeighborAgent->setNeighbors(this);  // add this agents to the neighbor list of the pointed Agent
    }
}

void Agent::setDirection(Vector2D direction)
{
    direction_=direction.getnormalized();
}


void Agent::ComputeNeighbors()
{
    for (std::shared_ptr<Agent> a:world->getCrowd())
    {
        if((position_-a->getPosition()).magnitude()<=world->getNeighborRange() && a.get() != this)
        {
            // if a agent is in Range and isn't the agent calling the function
            setNeighbors(a.get());
        }
    }

}

void Agent::UpdateNeighborsWithoutSearching()
{
    // What is the f#cking "NeighborList()" in UMANS

    // Here this function updates the neighbor list by checking the neighbors of the neighbors

    for(Agent* n:neighbors_)
    {
        for(Agent* nn:n->getNeighbors())
        {
            // For each neighbors of neighbors
            if((position_-nn->getPosition()).magnitude()<=world->getNeighborRange() && nn != this)
            {
                setNeighbors(nn);
            }
        }
    }

}

bool Agent::CheckContact(Agent* NeighborToCheck)
{
    bool contact_flag = false;
    
    if (distance(position_,NeighborToCheck->getPosition())<=Radius_a+NeighborToCheck->getRadius_a())
    // To be reset properly in order to work for elipsoid
        {
            contact_flag = true;
        }
    return contact_flag;
} 


bool Agent::CheckDownContact(Agent* NeighborDownToCheck)
{
    bool contact_flag = false;
    
    if (distance(DownPosition_,NeighborDownToCheck->getDownPosition())<Radius_a+NeighborDownToCheck->getRadius_a())
    // To be reset properly in order to work for elipsoid
        {
            contact_flag = true;
        }
    return contact_flag; 
}

Push* Agent::CreatePush(Agent* receiver)
{
    Push* p = new Push(this, receiver);
    //!// This pointer will have to be deleted in the contact model if clock > duration
    pushes_.insert(p);
    return p;
    // Create a push between the pusher and receiver and add it to the set of push
} 

void Agent::DeletePush( Push* PushToDelete)
{
    pushes_.erase(PushToDelete);
    delete PushToDelete;
}

std::vector<Vector2D> Agent::Stability( std::vector<Vector2D> X, Vector2D Fext) 
{
    Vector2D UpPosition = X[0];
    Vector2D UpVelocity= X[1];
    Vector2D DownPosition= X[2];
    Vector2D DownVelocity= X[3];
    
    float k1= 70 ;
    float kf = 2 ;
    float h1 = 20 ;
    float h2 = 10 ;


    // #### Solve the stability Systeme
    float H = 0.0;
    if ( (UpPosition-DownPosition).magnitude()<0.05 ){ H =1.0; }

    return std::vector<Vector2D> 
    {   UpVelocity,
        kf*(Fext/getMass()) - ( k1/getMass()*(UpPosition-DownPosition)*H + h1*UpVelocity ),
        DownVelocity,
        (1-H)*(std::max(2*getMaximumSpeed(),2.0f*std::abs(UpVelocity.magnitude()) )*(UpPosition-DownPosition).getnormalized() - DownVelocity) - H*h2*DownVelocity
    };

}


void Agent::Update(float timestep)
{
    Vector2D NullVector;

    // ComputeNeighbors
    //agent->ComputeNeighbors();
    for (std::shared_ptr<Agent> n:world->getCrowd())
    {
        setNeighbors(n.get());
        
    }
    std::vector<Vector2D> X0 = {position_,velocity_,DownPosition_,DownVelocity_}; // vector of the position and velocity at time t

    
    //#### Euler verlet integration :
    std::vector<Vector2D> deriv_vect = Stability( X0, ExtForces_); // Vector of derived quantities
    Vector2D pseude_up_velocity = velocity_+deriv_vect[1]*timestep;
    Vector2D pseude_down_velocity = DownVelocity_+deriv_vect[3]*timestep;
    std::vector<Vector2D> X1 ={position_ + pseude_up_velocity*timestep,
                                pseude_up_velocity,
                                DownPosition_ + pseude_down_velocity*timestep,
                                pseude_down_velocity};
                                // vector of the position and velocity at time t+timestep 


    //#### Check if there is conflict with another support basis
    
    // pseudo-update sopport position :
    Vector2D DownPos_at_t0 = DownPosition_; //save the position at time t
    setDownPosition(X1[2]); // Update sopport position
    
    // check conflicts 
    bool conflict_flag =false;
    for (Agent* n : neighbors_)
    {
            if(CheckDownContact(n))
            {
                conflict_flag=true;
                break;
            }

    }
    if( conflict_flag ) // if conflict then stop the lower part 
    {    
        std::cout << "conflict Update" << std::endl;
       setPosition(X1[0]);
       setVelocity(X1[1]); 
       setDownPosition(X0[0]);
       setDownVelocity(NullVector);
    }
    else
    {
        std::cout << "conflictless Update" << std::endl;
        setPosition(X1[0]);
        setVelocity(X1[1]);
        setDownPosition(X1[2]);
        setDownVelocity(X1[3]);

    }
    //#### finit length +fall
     if((position_-DownPosition_).magnitude()>epsilon)
    {
       setPosition(X1[0]);
       setVelocity(NullVector); 
       setDownPosition(X1[2]);
       setDownVelocity(X1[3]);
    } 


}