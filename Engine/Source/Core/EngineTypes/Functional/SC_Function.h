#pragma once
#include "EngineTypes/Containers/SC_Tuple.h"
#include <functional>

template<typename Signature>
using SC_Function = std::function<Signature>;

namespace SC_Placeholder
{
	inline constexpr std::_Ph<1> Arg1;
	inline constexpr std::_Ph<2> Arg2;
	inline constexpr std::_Ph<3> Arg3;
	inline constexpr std::_Ph<4> Arg4;
	inline constexpr std::_Ph<5> Arg5;
	inline constexpr std::_Ph<6> Arg6;
	inline constexpr std::_Ph<7> Arg7;
	inline constexpr std::_Ph<8> Arg8;
	inline constexpr std::_Ph<9> Arg9;
	inline constexpr std::_Ph<10> Arg10;
	inline constexpr std::_Ph<11> Arg11;
	inline constexpr std::_Ph<12> Arg12;
	inline constexpr std::_Ph<13> Arg13;
	inline constexpr std::_Ph<14> Arg14;
	inline constexpr std::_Ph<15> Arg15;
	inline constexpr std::_Ph<16> Arg16;
	inline constexpr std::_Ph<17> Arg17;
	inline constexpr std::_Ph<18> Arg18;
	inline constexpr std::_Ph<19> Arg19;
	inline constexpr std::_Ph<20> Arg20;
}

template<typename Signature>
struct SC_Function2;


namespace SC_CallbackInternal
{
	template<SC_SizeT... Indices>
	struct Invoker
	{
	};

	template<SC_SizeT Index, typename IndexArray>
	struct MakeInvoker;

	template<SC_SizeT Index, SC_SizeT... Indices>
	struct MakeInvoker<Index, Invoker<Indices...>>
	{
		typedef typename MakeInvoker<Index - 1, Invoker<Index - 1, Indices...>>::Type Type;
	};

	template<SC_SizeT... Indices>
	struct MakeInvoker<0, Invoker<Indices...>>
	{
		typedef Invoker<Indices...> Type;
	};
}

template<typename ReturnType, typename... Arguments>
struct SC_Function2<ReturnType(Arguments...)>
{
public:
	using Signature = ReturnType(Arguments...);
	using ArgsTuple = SC_Tuple<Arguments&&...>;

	struct Data;
	using Func = ReturnType(void*, const Data*, ArgsTuple&);

	struct Data
	{
		Data() : mInvoker(nullptr), mUserData(nullptr) {}
		Data(Func aInvoker, void* aUserData) : mInvoker(aInvoker), mUserData(aUserData) {}

		Func mInvoker;
		void* mUserData;
	} mData;

public:
	SC_Function2() {}
	SC_Function2(SC_Nullptr) {}

	template<typename Fn>
	SC_Function2(const Fn& aFunc, void* aUserData = nullptr) 
	{
		Set(aFunc, aUserData);
	}

	template<typename Object, typename MemberFn>
	SC_Function2(Object* aObject, MemberFn aMemberFunc)
	{
		static_assert(SC_IsMemberFunctionPointer<MemberFn>::gValue, "aMemberFunc is not a valid member function to aObject");
		Set(aObject, aMemberFunc);
	}

	template<typename FnObject>
	void Set(const FnObject& aFunctionObject, void* aUserData = nullptr)
	{
		// "incorrect argument to 'decltype'" means you've tried to use a function with an overloaded operator() which is forbidden.
		typedef decltype(&FnObject::operator()) FnObjectSignature;
		SetFunctionObject(aFunctionObject, static_cast<FnObjectSignature>(nullptr), aUserData);
	}

	ReturnType operator()(Arguments... aParams) const
	{
		SC_ASSERT(mData.mInvoker);
		ArgsTuple args = SC_ForwardAsTuple(SC_Forward<Arguments>(aParams)...);
		return mData.mInvoker(mData.mUserData, &mData, args);
	}

private:
	template<typename FnObject, typename RetVal, typename... FnArguments>
	void SetFunctionObject(const FnObject& aFunctionObject, RetVal(*)(FnArguments...), void* aUserData)
	{
		static_assert(sizeof...(FnArguments) <= sizeof...(Arguments), "Invalid arguments count");

		typedef typename SC_CallbackInternal::MakeInvoker<sizeof...(FnArguments), SC_CallbackInternal::Invoker<>>::Type FuncInvoker;
		Func func = &FuncInvoker::template WrapFunctionObject<ReturnType, Data, ArgsTuple, FnObject, RetVal, FnArguments...>;
		this->mData = Data(func, aFunctionObject, aUserData);
	}

	template<typename FnObject, typename RetVal, typename Obj, typename... FnArguments>
	void SetFunctionObject(const FnObject& aFunctionObject, RetVal(Obj::*)(FnArguments...), void* aUserData)
	{
		static_assert(sizeof...(FnArguments) <= sizeof...(Arguments), "Invalid arguments count");

		typedef typename SC_CallbackInternal::MakeInvoker<sizeof...(FnArguments), SC_CallbackInternal::Invoker<>>::Type FuncInvoker;
		Func func = &FuncInvoker::template WrapFunctionObject<ReturnType, Data, ArgsTuple, FnObject, RetVal, FnArguments...>;
		this->mData = Data(func, aFunctionObject, aUserData);
	}

	//template<typename FnObject, typename RetVal, typename Obj, typename... FunctionArguments>
	//void SetFunctionObject(const FnObject& aFunctionObject, RetVal(Obj::*)(FunctionArguments...) const, void* aUserData)
	//{
	//	static_assert(sizeof...(FunctionArguments) <= sizeof...(Arguments), "The function contains more arguments than MC_Callback");
	//
	//	typedef typename MC_Callback_Internal::MakeInvoker<sizeof...(FunctionArguments), MC_Callback_Internal::Invoker<>>::Type FuncInvoker;
	//	Func func = &FuncInvoker::template WrapFunctionObject<Ret, Data, ArgumentTuple, FunctionObject, RetVal, FunctionArguments...>;
	//	Data d(func, aFunctionObject, aUserData);
	//	this->myData = d;
	//}
};