#include "stdafx.h"

OutputSocket::OutputSocket()
{
}

OutputSocket::~OutputSocket()
{
	mAssignedNode->Value(false);
}

// Method called whenever the Event this object is subscribed to is raised.
void OutputSocket::Callback()
{
	// Propagate the change of state
	InternalStateChanged();
}


// Setter of the value of this socket
void OutputSocket::Value(bool value)
{
	if (mAssignedNode != nullptr)
	{
		mAssignedNode->Value(value);
		InternalStateChanged();
	}
}


// Generates information about the output socket in the following format (or similiar,
// depending on the state):
//		ID: X
//		Status: ON
//		Press f to create connection
//
std::list<Line> OutputSocket::GenerateInfo()
{
	return std::list<Line>
	{
		Line("ID: " + std::to_string(ID()), INDENT),
		Line("(QC) Connect", INDENT),
		GenerateStatusLine(Value(), INDENT),
	};
}


// Generates information about the output socket in the following format (or similiar,
// depending on the state):
// "
//	
//		Status: ON
// "
std::list<Line> OutputSocket::GenerateStatus()
{
	return std::list<Line>
	{
		Line(),
		Line(),
		GenerateStatusLine(Value(), INDENT),
	};
}


// Performs quick action for the output socket - quick connect command input
void OutputSocket::QuickAction()
{
	QuickActionCommand("Connect " + std::to_string(ID()) + ",");
}


// Compacts all the info into a single line (useful for parts with many inputs)
Line OutputSocket::GenerateInfoCompact()
{
	std::string info = "ID:" + std::to_string(ID()) + " ";

	if (info.length() < 20)
	{
		info.append(20 - info.length(), ' ');
	}

	return Line(info, INDENT, info.length() - 2, 1, Value() ? GREEN*BG : RED*BG);
}