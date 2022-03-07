#include "Push.h"


Push::Push(Agent* pusher, Agent* receiver) :
 Pusher(pusher), Receiver(receiver)
{
}

void Push::ClockTick(float dt)
{
    Clock = Clock + dt ;
}