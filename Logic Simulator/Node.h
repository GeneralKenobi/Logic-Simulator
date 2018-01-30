#pragma once
#include "stdafx.h"


// Node is the most basic element in the chierarchy. Nodes are owned by OutputSockets.
// Each OutputSocket is created with a node that can't be overwritten. That node can then
// be shared with other Sockets (InputSockets to be precise) which will allow to
// share a common state between all Sockets that are connected. However only OutputSockets may modify
// the state of a node
class Node : public Identifiable
{
 
	#pragma region Friendship declarations

	// BaseParts need to be able to assign its ownership of this node (so that it's possible to later print
	// that an input is connected to a certain part) 
	friend void BasePart::AssignNodeOwnership(std::shared_ptr<Node> node, std::shared_ptr<OutputSocket> socket);

	// OutputSocket owns the Node and it has to be granted access to its members in order to be able to modify them
	friend class OutputSocket;

	#pragma endregion

private:

	#pragma region Members

	// Value assigned to this node. The shortest amount of time between updates
	// is 10ms (Only the last value in that interval will be assigned)
	bool mValue = false;

	// When true, this node is ready to update (There's a minimum 10ms delay between
	// updates in order to avoid StackOverflowExcpetions due to infinite feedback loops, for example when a NOT gate
	// is connected to itself)
	bool mReadyToUpdate = true;

	// The last caught value which will be set when the current Delay has ended
	bool mLastCaughtValue;

	// ID of the part that "owns" this Node (i.e. part whose OutputSocket owns this node)
	int mOwnerID;

	// ID of the OutputSocket that owns this Node
	int mOutputSocketID;

#pragma endregion

	#pragma region Methods

	// Delays the update of this Node
	void Delay();

	#pragma endregion

	#pragma region Accessors

	// Setter of the value assigned to this node
	void Value(bool value);

	#pragma endregion

public:

	#pragma region Accessors

	// Getter to the value assigned to this node
	bool Value() { return mValue; };
	
	// Getter to the ID of the part that "owns" this Node
	int OwnerID() { return mOwnerID; }

	// Getter to the ID of the OutputSocket that "owns" this Node
	int OuputSocketID() { return mOutputSocketID; }

	#pragma endregion

	#pragma region Internal State Changed Event

	// Event fired whenever Value of this node changes
	const Event<> InternalStateChanged;

	#pragma endregion

	#pragma region Constructors / Destructors

	// Default destructor
	virtual ~Node() { }

	#pragma endregion

};