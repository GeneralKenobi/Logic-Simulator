// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once




#include "targetver.h"

#include <filesystem>
#include <deque>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>
#include <map>
#include <list>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <memory>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>


#include "ForwardDeclarations.h"
#include "Defines.h"
#include "StringHelpers.h"
#include "Line.h"
#include "Exception.h"
#include "Identifiable.h"
#include "Event.h"
#include "IEventSubscriber.h"
#include "BasePart.h"
#include "Node.h"
#include "OutputSocket.h"
#include "InputSocket.h"
#include "FunctionHelpers.h"
#include "BaseMultiInputGate.h"
#include "Propagator.h"
#include "BaseInput.h"
#include "Output.h"
#include "ORGate.h"
#include "NORGate.h"
#include "ANDGate.h"
#include "NANDGate.h"
#include "XORGate.h"
#include "XNORGate.h"
#include "NOTGate.h"
#include "BaseMuxDmux.h"
#include "MUX.h"
#include "DMUX.h"
#include "BaseLatch.h"
#include "DLatch.h"
#include "DFlipFlop.h"
#include "TLatch.h"
#include "TFlipFlop.h"
#include "JKLatch.h"
#include "JKFlipFlop.h"
#include "ToggleInput.h"
#include "ClockInput.h"
#include "ButtonInput.h"
#include "FlowControl.h"
#include "CustomLogic.h"
#include "Commands.h"
#include "PrintManager.h"

// TODO: reference additional headers your program requires here
