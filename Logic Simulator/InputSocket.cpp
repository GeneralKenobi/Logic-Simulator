#include "stdafx.h"



// Method called whenever the Event this object is subscribed to is raised.
void InputSocket::Callback()
{
	// Propagate the change of state
	InternalStateChanged();
}


// Setter of this Socket's node
void InputSocket::AssignedNode(std::weak_ptr<Node> value)
{
	// If we already had an input assigned here:
	if (!mNode.expired())
	{
		// Unsubscribe from the InternalStateChanged event
		mNode.lock()->InternalStateChanged.Unsubscribe(
			std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()));
	}

	mNode = value;

	// Assign the value and subscribe to InternalStateChanged
	if (!value.expired())
	{
		mNode.lock()->InternalStateChanged.Subscribe(
			std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()), std::bind(&InputSocket::Callback, this));
	}	

	InternalStateChanged();
}


// Generates information about the output socket in the following format (or similiar,
// depending on the state): 
//		ID: X
//		Status: ON
//		Press f to create connection
//
std::list<Line> InputSocket::GenerateInfo()
{
	std::string connection;

	// Info about the connection
	if (IsConnected())
	{
		std::shared_ptr<BasePart> owner;
		std::string ownerName;

		if (Identifiable::TryGetIDOwner<BasePart>(ConnectedTo(), owner))
		{
			ownerName = "(" + owner->PrintName() + ")";
		}

		connection = "Connected to: " + std::to_string(ConnectedTo()) +
			ownerName + " (QA) Disconnect";
	}
	else
	{
		connection = "Not connected (QC) Connect";
	}

	return std::list<Line>
	{
		Line("ID: " + std::to_string(ID()), INDENT),
		Line(connection, INDENT),
		GenerateStatusLine(Value(), INDENT),
	};
}


// Compacts all the info into a single line (useful for parts with many inputs)
Line InputSocket::GenerateInfoCompact()
{
	std::string info = "ID:" + std::to_string(ID()) + (IsConnected() ?
		" Con.:" + std::to_string(ConnectedTo()) : " Discon.") + " ";

	if(info.length() < 20)
	{
		info.append(20 - info.length(), ' ');
	}

	return Line(info, INDENT, info.length() - 2, 1, Value() ? GREEN*BG : RED*BG);
}


// Generates information about the output socket in the following format (or similiar,
// depending on the state): 
//		
//		Status: ON
//	
std::list<Line> InputSocket::GenerateStatus()
{
	return std::list<Line>
	{
		Line(),
		Line(),
		GenerateStatusLine(Value(), INDENT),
	};
}


// Performs quick action for the input socket - disconnect if connected or quick connect command input if disconnected
void InputSocket::QuickAction()
{
	if (IsConnected())
	{
		mNode.reset();
		PrintManager(Sector::PartInfo);
	}
	else
	{
		QuickActionCommand("Connect " + std::to_string(ID()) + ",");
	}
}


// Generates a string that contains connection info ("x,y;" for connected or "" for unconnected)
std::string InputSocket::GenerateConnectionString()
{
	if (IsConnected())
	{
		return std::to_string(-ID()) + "," + std::to_string(-ConnectedSocketID()) + ";";
	}
	else
	{
		return std::string();
	}
}
