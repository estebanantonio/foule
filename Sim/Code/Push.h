#ifndef Push_H
#define Push_H
#include <memory>
#include <set>

#include"Agent.h"
#include "World.h"

class Agent;


class Push
{

    Agent* Pusher; // Pointer on the Pusher
    Agent* Receiver; // Pointer on the Receiver
    float Clock = 0.0f; // tic at every time step
    //float Duration; // after that time the Push will be deleted

public:
    Push(Agent* pusher, Agent* receiver);
    void ClockTick(float dt); // Iterate the internal clock of the puch to keep track of the reaction of the agents (the agents react even after the push)
    
    // getters
    Agent* getPusher() const {return Pusher;} ;
    Agent* getReceiver() const {return Receiver;} ;
    float getClock() const {return Clock;};



};



#endif // Push_H