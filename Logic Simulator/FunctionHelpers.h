#pragma once
#include "stdafx.h"

// Returns the address of a function (doesn't work for methods)
template <typename T, typename... Args>
size_t GetFunctionAddress(std::function<T(Args...)> f)
{
	typedef T(ft)(Args...);
	ft ** fPtr = f.template target<ft*>();
	return (size_t)*fPtr;
}

