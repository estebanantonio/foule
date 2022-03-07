#include<iostream>
#include <memory>
#include <sstream>

using namespace std;

#include "vector2D.h"
#include "Agent.h"
#include "Push.h"
#include"World.h"
#include "InOutClass.h"

#include "InOutClass.cpp"
#include "Agent.cpp"
#include "Push.cpp"
#include"World.cpp"


int main()
{    
    // #### Generate the world
    World world; 

    // #### Set world parameters
    world.SetNeighborRange(0.8);
    world.SetTimeStep( 0.2 *(world.getAvgAgentRadius_a()*2.0/1.8) ); // Set the time step base on 0.5 of cfl crit

    // Set and initialize the input and output of the world
    InOutClass IOF("none.txt", "test.txt",&world); 


    // Create the agents of the crowd
    Agent* a0 = world.CreateAgent(0,0,0);
    
    for (int i = 1; i < 1; i++)
    {
        world.CreateAgent(i,i*0.65,0);
    }
    



    // Initialization
    IOF.WriteIteration(0);


    // Main Loop
    int i=0;
    float dt =world.getTimeStep();
    while (world.getClock()<=1)
    {
        i++;
        cout << "############################# Time : " << world.getClock() <<"s #############################" << endl;
        //cout << "#####################     Timestep : " << world.getTimeStep()<<"s ######################" << endl;
        world.Iterate();
        IOF.WriteIteration(i);
    }
    


    return 0;
} 