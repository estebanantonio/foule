#ifndef World_H
#define World_H
#include <memory>
#include <sstream>
#include <set>
#include <math.h>

#include"Agent.h"
#include "Push.h"


class World
{
    std::set<std::shared_ptr<Agent>> Crowd;
    // std::set<std::shared_ptr<Walls*>> Boundaries;
    

    // Parameters
    float Clock = 0.0f; // global Clock of the sim, ticked at every Timestep
    float TimeStep = 0.1f; // Time step set at 0.1s by default
    float NeighborRange = 2.0f; // By default, neighbors are agents at less than 1m
    float AvgAgentRadius_a = 0.3f; // 1st radius of the elipsoid that represent the upper part of the body
    float AvgAgentRadius_b = 0.3f; // 2nd radius of the elipsoid that represent the upper part of the body
    float AvgReactionTime = 0.45f; // Time after which the agents will respond to contacts
    float AvgMass = 75; // Average mass of the agents
    
    
    // Parameters of the contact model
    float k_momentum = 1.0f; // Coeff of momentum attenuation while transfer during contact
    float k_power = 1.0f; // Coeff of acceleration attenuation while transfer during active pushing
    

public:
    World();

    void Iterate(); // Change the state of the world to Iteration N+1

    Agent* CreateAgent(int id, float pos_x, float pos_y);

    void SolveSimpleContact(Agent* agent); // previous contact model implement in humans
    
    void ComputeContacts(Agent* agent); 
    void SolveContacts(Agent* agent);
    void ActiveContact(Push* push); // Solve the active reaction of the pusher onto the receiver
    void PassiveContact(Push* push); // Solve the  passive reaction of the pusher onto the receiver

    // TOOLS :
    std::string ListAgents() const;
        float ComputeNextTimeStep(float CurrentTimeStep, 
                                    float SaftyCoef, 
                                    float AmpliCoeff, 
                                    float MaxTimeStep);
    void ClockTick(float dt); // Tick the clock with a given TimeStep
    // std::string ListWalls() const;
    void SetNeighborRange(float NR) {NeighborRange = NR;};
    void SetTimeStep(float dt) { TimeStep = dt;};
    float getNeighborRange(){return NeighborRange;};
    float getTimeStep() const {return TimeStep;};
    float getClock() const {return Clock;};
    float getAvgAgentRadius_a() const {return AvgAgentRadius_a;};
    float getAvgAgentRadius_b() const {return AvgAgentRadius_b;};
    float getAvgMass() const {return AvgMass;};
    std::set<std::shared_ptr<Agent>> getCrowd(){return Crowd;};
    

};



#endif // World_H