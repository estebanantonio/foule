#ifndef agent_H
#define agent_H
// Header de la class (aussi appele prototype de la class)


#include <set>
#include <memory>
#include <sstream>
#include <vector>
#include <math.h>

#include "vector2D.h"
#include "World.h"
#include "Push.h"



class Agent
{
public:
	/// <summary>A struct containing the settings of an agent that typically do not change over time.</summary>
	struct settings
	{
		/// <summary>The radius of the agent's disk representation (in meters).</summary>
		//float radius_ = 0.24f;
		/// <summary>The preferred walking speed of the agent (in meters per second).</summary>
		float preferred_speed_ = 1.4f;
		/// <summary>The maximum walking speed of the agent (in meters per second).</summary>
		float max_speed_ = 2.0f;
		/// <summary>The maximum acceleration of the agent (in meters per second squared).</summary>
		/// <remarks>To allow abrupt changes in velocity, use a high value, 
		/// combined with a low relaxation time in the agent's Policy.</remarks>
		float max_acceleration_ = MaxFloat;

		/// <summary>A pointer to the policy that describes the agent's navigation behavior.</summary>
		//Policy* policy_ = nullptr;

		/// <summary>Whether or not the agent should be removed when it has reached its goal.</summary>
		bool remove_at_goal_ = false;

		/// <summary>The mass of the agent, used when applying an acceleration vector. A commonly used value is 1, so this is not a mass in kilograms.</summary>
		float mass_ = 60.0f;
	};
    

private:

	size_t id_;
	Agent::settings settings_;
	Vector2D position_;
	Vector2D velocity_;
	Vector2D acceleration_;

	std::set<Agent*> neighbors_;

	// New Attributs 
	World* world; // World in wich the Agent exists
	Vector2D DownPosition_; // Possition of the Base of Support
	Vector2D DownVelocity_; // Velocity of the Base of Support
	Vector2D DownAcceleration_; // Acceleration of the Base of Support
	Vector2D ExtForces_; // Extenal forces comming from the Pushes
	Vector2D direction_; // direction of the agents set along the x axix by default (define the angle of the elipsoid that rpz the agents)
	float Radius_a;
	float Radius_b;
	float epsilon = 0.5f; // Distance between basis and Com at wich the agent doesn't try the regain balance anymore.
	std::set<Push*> pushes_; // set of pushes the agent is giving the position
	

	// Functions

public :
	Agent(World* w, int id, float pos_x, float pos_y);
	void ComputeNeighbors(); // Check all the agents in the World and add them to "neighbors_" if there are in the neighborrange 
	void UpdateNeighborsWithoutSearching();	// Check only the "neighbors_" of the current Neighbors
	
	// New Functions
	std::vector<Vector2D> Stability(std::vector<Vector2D> X, Vector2D Fext) ; // Returns the acceleration of the Low an upper part based on the curent Velocity and Possition
	bool CheckContact(Agent* NeighborToCheck); // Return True if there is contact between the agent and the considered neighbor
	bool CheckDownContact(Agent* NeighborDownToCheck); // Return True if there is contact between Down of the agent and one of the considered neighbor
	Push* CreatePush( Agent* receiver); // Creat a new push this agent is giving to the receiver and return a pointer at it
	void DeletePush( Push* PushToDelete); // delete the push and remove it from pushes_

	// Tool Functions
	std::string ToString() const; // Returns a string containing the Current position and velocity of the agent
	void Update(float timestep); // Update the current Down and CoM position/velocity base on 1st order Euler sheme

	void setNeighbors(Agent* NeighborAgent); // Add a pointer on an agent to the Neighbors list
	void setDirection(Vector2D direction);
	void setMass(float mass){settings_.mass_=mass;};
	void setExtForces(Vector2D forces){ExtForces_=forces;};
	
	void setAcceleration(Vector2D acceleration){acceleration_=acceleration;};
	void setVelocity(Vector2D velocity){velocity_=velocity;};
	void setPosition(Vector2D position){position_=position;};

	void setDownAcceleration(Vector2D acceleration){DownAcceleration_=acceleration;};
	void setDownVelocity(Vector2D velocity){DownVelocity_=velocity;};
	void setDownPosition(Vector2D position){DownPosition_=position;};

	// New getters
	Vector2D getExtForces() const{ return ExtForces_;};
	Vector2D getDirection() const{ return direction_;};
	float getRadius_a() const{ return Radius_a;};
	float getRadius_b() const{ return Radius_b;};
	std::set<Push*> getPushes() const{ return pushes_;};

	Vector2D getAcceleration() const{ return acceleration_;};

	Vector2D getDownAcceleration() const{ return DownAcceleration_;};
	Vector2D getDownVelocity() const{ return DownVelocity_;};
	Vector2D getDownPosition() const{ return DownPosition_;};

#pragma region [Basic getters]
	/// @name Basic getters
	/// Methods that directly return a value stored in the agent.

	/// <summary>Returns the unique ID of this agent.</summary>
	size_t getID() const { return id_; }
	/// <summary>Returns the agent's current position.</summary>
	inline const Vector2D& getPosition() const { return position_; }
	/// <summary>Returns the agent's last used velocity.</summary>
	inline const Vector2D& getVelocity() const { return velocity_; }
	/// <summary>Returns the radius of the agent's disk representation.</summary>
	//inline float getRadius() const { return settings_.radius_; }
	/// <summary>Returns the agent's preferred walking speed.</summary>
	inline float getPreferredSpeed() const { return settings_.preferred_speed_; };
	/// <summary>Returns the agent's maximum walking speed.</summary>
	inline float getMaximumSpeed() const { return settings_.max_speed_; };
	/// <summary>Returns the agent's maximum acceleration.</summary>
	inline float getMaximumAcceleration() const { return settings_.max_acceleration_; };
	/// <summary>Returns the agent's mass.</summary>
	inline float getMass() const { return settings_.mass_; };
	/// <summary>Returns the (most recently computed) list of neighbors for this agent.</summary>
	/// <returns>A non-mutable reference to the list of neighbors that this agent has last computed.</returns>
	std::set<Agent*> getNeighbors() const { return neighbors_; }


	/// @}
#pragma endregion





};

#endif //agent_H