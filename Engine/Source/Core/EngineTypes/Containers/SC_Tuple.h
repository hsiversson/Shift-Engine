#pragma once

#include "Platform/Misc/SC_CommonFunctions.h"

template<class... Types>
struct SC_Tuple
{
};

template<typename Type, typename... OtherTypes>
struct SC_Tuple<Type, OtherTypes...> : public SC_Tuple<OtherTypes...>
{
	SC_Tuple(Type aValue, OtherTypes... aOtherTypes)
		: SC_Tuple<OtherTypes...>(SC_Forward<OtherTypes...>(aOtherTypes)...)
		, mValue(SC_Forward<Type>(aValue))
	{}

	Type mValue;
};

template<typename... Types>
constexpr SC_Tuple<Types&&...> SC_ForwardAsTuple(Types&&... aArgs)
{
	return SC_Tuple<Types&&...>(SC_Forward<Types>(aArgs)...);
}