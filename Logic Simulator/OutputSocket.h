#pragma once
#include "stdafx.h"

// OutputSocket is a socket which has its Node from the beginning and it
// cannot be overwritten. It's value can be set by this socket.
class OutputSocket : public Identifiable, public IEventSubscriber<>
{

public:

	#pragma region Constructor/Destructor

	OutputSocket();

	virtual ~OutputSocket();

	#pragma endregion

	#pragma region Members

	// Node of this OutputSocket
	const std::shared_ptr<Node> mAssignedNode = Identifiable::Create<Node>();

	#pragma endregion

	#pragma region Internal State Changed Event

	// Event triggered whenever Value of this Socket changes
	const Event<> InternalStateChanged;

	#pragma endregion

	#pragma region IEventSubscriber Callback

	// Method called whenever the Event this object is subscribed to is raised.
	virtual void Callback();	

	#pragma endregion

	#pragma region Accessors
	
	// Setter of the value of this socket
	void Value(bool value);

	// Getter to the value of this Socket
	bool Value() { return mAssignedNode == nullptr ? false : mAssignedNode->Value(); }

	#pragma endregion

	#pragma region Methods

	// Generates information about the output socket in the following format (or similiar,
	// depending on the state):
	// "Output:
	//		ID: X
	//		Status: ON
	//		Press f to create connection
	std::list<Line> GenerateInfo();

	// Generates information about the output socket in the following format (or similiar,
	// depending on the state):
	// "
	//	
	//		Status: ON
	// "
	std::list<Line> GenerateStatus();

	// Compacts all the info into a single line (useful for parts with many inputs)
	Line GenerateInfoCompact();

	// Performs quick action for the output socket - quick connect command input
	void QuickAction();

	#pragma endregion
};