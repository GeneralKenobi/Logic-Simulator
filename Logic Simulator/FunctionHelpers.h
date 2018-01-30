#pragma once
#include "stdafx.h"

// Returns the address of a function (doesn't work for methods)
template <typename T, typename... Args>
size_t GetFunctionAddress(std::function<T(Args...)> f)
{
	typedef T(fnType)(Args...);
	fnType ** fnPointer = f.template target<fnType*>();
	return (size_t)*fnPointer;
}

