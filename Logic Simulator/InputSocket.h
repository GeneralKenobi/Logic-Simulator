#pragma once
#include "stdafx.h"

// InputSocket is a socket which can be connected with an OutputSocket.
// It's node can be assigned and reassigned at will. It can't change the Node's value.
class InputSocket : public Identifiable, public IEventSubscriber<>
{

private:
	
	#pragma region Members

	// Node to which this Socket is connected
	std::weak_ptr<Node> mNode;

	#pragma endregion

public:

	#pragma region Constructors/Destructors

	// Default destructor
	~InputSocket() { }

	#pragma endregion

	#pragma region Internal State Changed Event

	// Event triggered whenever Value of this Socket changes
	const Event<> InternalStateChanged;

	#pragma endregion

	#pragma region IEventSubscriber Callback

	// Method called whenever the Event this object is subscribed to is raised.
	void Callback();

	#pragma endregion

	#pragma region Accessors/Getters

	// Getter to the value of this Socket
	bool Value() { return mNode.expired() ? false : mNode.lock()->Value(); }

	// Returns true if this socket is connected to an output
	bool IsConnected() { return !mNode.expired(); }

	// Setter of this Socket's node
	// partName is the display name of the part which owns the Node
	void AssignedNode(std::weak_ptr<Node> value);	

	// Returns ID of the part it's connected to, -1 if it's not connected
	int ConnectedTo() { return IsConnected() ? mNode.lock()->OwnerID() : -1; }

	// Returns the ID of the output socket it's connected to or -1 if it's not connected
	int ConnectedSocketID() { return IsConnected() ? mNode.lock()->OuputSocketID() : -1; }

	#pragma endregion

	#pragma region Methods
	
	// Generates information about the output socket in the following format (or similiar,
	// depending on the state): 
	//		ID: X
	//		Status: ON
	//		Press f to create connection
	std::list<Line> GenerateInfo();

	// Compacts all the info into a single line (useful for parts with many inputs)
	Line GenerateInfoCompact();

	// Generates information about the output socket in the following format (or similiar,
	// depending on the state): 
	//		
	//		Status: ON
	//		
	std::list<Line> GenerateStatus();

	// Performs quick action for the input socket - disconnect if connected or quick connect command input if disconnected
	void QuickAction();

	// Generates a string that contains connection info ("x,y;" for connected or "" for unconnected)
	std::string GenerateConnectionString();
	
	#pragma endregion

};