#include "stdafx.h"



// Initial routine - subscribes to Input and Output InternalStateChanged with RefreshInfoStatus
void CustomLogic::InitializeRoutine()
{
	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		(*it)->Output()->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
			std::bind(&BasePart::RefreshInfoStatus, this));
	}

	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it)
	{
		(*it)->Output()->InternalStateChanged.Subscribe(std::dynamic_pointer_cast<IEventSubscriber>(shared_from_this()),
			std::bind(&BasePart::RefreshInfoStatus, this));
	}
}


// Assigns node ownership to every output socket in mOutputs
void CustomLogic::AssignOwnership()
{
	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it)
	{
		AssignNodeOwnership((*it)->Output()->mAssignedNode, (*it)->Output());
	}
}


// Default constructor which allows to assign a name to this CustomLogic
CustomLogic::CustomLogic(std::string name)
{
	mPrintName = name;
}


// Constructor which creates a CustomLogic with a set of parts that it should store. Finds and exposes marked Propagators
CustomLogic::CustomLogic(std::list<std::shared_ptr<BasePart>> subParts,	std::string name) : mSubParts(subParts)
{
	mPrintName = name;

	for (auto it = subParts.begin(); it != subParts.end(); ++it)
	{
		if (auto cast = std::dynamic_pointer_cast<Propagator>(*it))
		{
			if(cast->OperationMode()==2)
			{
				mOutputs.push_back(cast);
			}
			else if (cast->OperationMode()==1)
			{
				mInputs.push_back(cast);
			}
		}
	}
}

int CustomLogic::SelectableLines()
{
	return 8 + 4*(mInputs.size() + mOutputs.size());
}

std::list<Line> CustomLogic::GenerateInfo()
{
	std::list<Line> l = BasePart::GenerateInfo();
	l.push_back(Line());
	l.push_back(Line("Inputs:"));
	l.push_back(Line());

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		l.splice(l.end(),(*it)->GenerateInfo());
		l.push_back(Line());
	}

	l.push_back(Line());
	l.push_back(Line("Outputs:"));
	l.push_back(Line());

	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it)
	{
		l.splice(l.end(), (*it)->GenerateInfo());
		l.push_back(Line());
	}

	return l;
}

std::list<Line> CustomLogic::GenerateStatus()
{
	std::list<Line> l = BasePart::GenerateStatus();
	l.push_back(Line());
	l.push_back(Line());
	l.push_back(Line());

	for (auto it = mInputs.begin(); it != mInputs.end(); ++it)
	{
		l.splice(l.end(), (*it)->GenerateStatus());
		l.push_back(Line());
	}

	l.push_back(Line());
	l.push_back(Line());
	l.push_back(Line());

	for (auto it = mOutputs.begin(); it != mOutputs.end(); ++it)
	{
		l.splice(l.end(), (*it)->GenerateStatus());
		l.push_back(Line());
	}

	return l;
}


void CustomLogic::QuickActionRequest(int index)
{
	switch (index)
	{
		case 0:
		case 1:
		case 2:
		{
			BasePart::QuickActionRequest(index);
		}
		break;

		//case 8,12...
		default:
		{
			if (index % 4 == 3 && index > 4 && index < (mInputs.size() * 4 + 5))
			{
				// Case for input
				auto it = mInputs.begin();
				std::advance(it, (index-7)/4);
				(*it)->QuickActionRequest(0);
			}
			else if (index % 4 == 2 && index > (mInputs.size() * 4 + 7) && index < SelectableLines())
			{
				// Case for outputs
				auto it = mOutputs.begin();
				std::advance(it, (index - mInputs.size()*4 - 10) / 4);
				(*it)->QuickActionRequest(0);
			}
		}

	}
}


// Generates string to save in a file which allows to recreate this part. Gathers information on all parts that
// make up this CustomLogic
std::vector<std::string> CustomLogic::GenerateStorageString()
{
	std::string partsInfo;
	std::string connections;

	for (auto it = mSubParts.begin(); it != mSubParts.end(); ++it)
	{
		auto v = (*it)->GenerateStorageString();

		partsInfo += v[1] + '|' + v[2] + '\n';

		if (v[0]!="")
		{
			connections+=v[0] + ';';
		}
	}

	if (!partsInfo.empty())
	{
		partsInfo = partsInfo.substr(0, partsInfo.size()-1);
	}

	if (!connections.empty())
	{
		connections = connections.substr(0, connections.size() - 1);
	}


	return std::vector<std::string> {connections,partsInfo};
}
