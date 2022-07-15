#pragma once
#include <type_traits>

//-------------------------------------------------------------//
//	Types Equal

template<typename A, typename B>
struct SC_AreTypesEqual;

template<typename, typename>
struct SC_AreTypesEqual
{
	enum { mValue = false };
};

template<typename A>
struct SC_AreTypesEqual<A, A>
{
	enum { mValue = true };
};

#define SC_ARE_TYPES_EQUAL(A,B) SC_AreTypesEqual<A,B>::mValue

//-------------------------------------------------------------//
//	Is Reference
template<typename T> struct SC_IsReference { enum { mValue = false }; };
template<typename T> struct SC_IsReference<T&> { enum { mValue = true }; };
template<typename T> struct SC_IsReference<T&&> { enum { mValue = true }; };

template<typename T> struct SC_IsLeftValueReference { enum { mValue = false }; };
template<typename T> struct SC_IsLeftValueReference<T&> { enum { mValue = true }; };

template<typename T> struct SC_IsRightValueReference { enum { mValue = false }; };
template<typename T> struct SC_IsRightValueReference<T&&> { enum { mValue = true }; };

//-------------------------------------------------------------//
//	Remove Reference 

template<typename T>
struct SC_RemoveReference { typedef T Type; };

template<typename T>
struct SC_RemoveReference<T&> { typedef T Type; };

template<typename T>
struct SC_RemoveReference<T&&> { typedef T Type; };

template <typename T>
struct SC_RemoveConst { typedef T Type; };

template <typename T>
struct SC_RemoveConst<const T> { typedef T Type; };

//-------------------------------------------------------------//
//	Intergral Constant 

template <class T, T Val>
struct SC_IntegralConstant
{
	static constexpr T mValue = Val;

	using ValueType = T;
	using Type = SC_IntegralConstant;

	constexpr operator ValueType() const noexcept
	{
		return mValue;
	}

	constexpr ValueType operator()() const noexcept
	{
		return mValue;
	}
};

//-------------------------------------------------------------//
//	Bool Constant 

template <bool Val>
using SC_BoolConstant = SC_IntegralConstant<bool, Val>;
using SC_TrueType = SC_BoolConstant<true>;
using SC_FalseType = SC_BoolConstant<false>;

//-------------------------------------------------------------//
//	Destructible

template <class T>
struct SC_IsTriviallyDestructible : SC_BoolConstant<__is_trivially_destructible(T)> {};

template <class T>
inline constexpr bool SC_IsTriviallyDestructibleValue = __is_trivially_destructible(T);

//-------------------------------------------------------------//
//	Copyable

namespace SC_CopyDisabledNamespace
{
	class SC_CopyDisabled
	{
	protected:
		SC_CopyDisabled() {}
		~SC_CopyDisabled() {}
	private:
		SC_CopyDisabled(const SC_CopyDisabled&);
		const SC_CopyDisabled& operator=(const SC_CopyDisabled&);
	};
}
typedef SC_CopyDisabledNamespace::SC_CopyDisabled SC_CopyDisabled;

template <class T>
struct SC_IsTriviallyCopyable : SC_BoolConstant<__is_trivially_copyable(T)> {};

template <class T>
inline constexpr bool SC_IsTriviallyCopyableValue = __is_trivially_copyable(T);

//-------------------------------------------------------------//
//	Detection Idiom 

namespace SC_DetectionIdiomNamespace
{
	struct SC_NoneSuch
	{
		~SC_NoneSuch() = delete;
		SC_NoneSuch(SC_NoneSuch const&) = delete;
		void operator=(SC_NoneSuch const&) = delete;
	};

	template <class Default, class AlwaysVoid, template<class...> class Op, class... Args>
	struct SC_Detector
	{
		using ValueType = SC_FalseType;
		using Type = Default;
	};

	template <class Default, template<class...> class Op, class... Args>
	struct SC_Detector<Default, std::void_t<Op<Args...>>, Op, Args...>
	{
		using ValueType = SC_TrueType;
		using Type = Op<Args...>;
	};

}

template <template<class...> class Op, class... Args>
using SC_DetectionIdiom = typename SC_DetectionIdiomNamespace::SC_Detector<SC_DetectionIdiomNamespace::SC_NoneSuch, void, Op, Args...>::ValueType;

//-------------------------------------------------------------//
//	Pointer Traits

template <class T, class = void>
struct SC_CanScalarDelete : SC_FalseType {};
template <class T>
struct SC_CanScalarDelete<T, std::void_t<decltype(delete std::declval<T*>())>> : SC_TrueType {};

template <class T, class = void>
struct SC_CanArrayDelete : SC_FalseType {};
template <class T>
struct SC_CanArrayDelete<T, std::void_t<decltype(delete[] std::declval<T*>())>> : SC_TrueType {};

template <class Fn, class Arg, class = void>
struct SC_CanCallFunctionObject : SC_FalseType {};
template <class Fn, class Arg>
struct SC_CanCallFunctionObject<Fn, Arg, std::void_t<decltype(std::declval<Fn>()(std::declval<Arg>()))>> : SC_TrueType {};

//-------------------------------------------------------------//
//	Function Traits

template <typename T> struct SC_RemoveFnConstVolatileReference { typedef T Type; };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) const> { typedef Fn Type(Args...); };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) const&> { typedef Fn Type(Args...); };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) const&&> { typedef Fn Type(Args...); };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) volatile const> { typedef Fn Type(Args...); };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) volatile const&> { typedef Fn Type(Args...); };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) volatile const&&> { typedef Fn Type(Args...); };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) volatile > { typedef Fn Type(Args...); };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) volatile&> { typedef Fn Type(Args...); };
template <typename Fn, class... Args> struct SC_RemoveFnConstVolatileReference<Fn(Args...) volatile&&> { typedef Fn Type(Args...); };

template <typename T> struct SC_IsFunctionInternal { static constexpr const bool gValue = false; };
template <class Ret, class... Args> struct SC_IsFunctionInternal<Ret(Args...)> { static constexpr const bool gValue = true; };
template <class T> struct SC_IsFunction : SC_IsFunctionInternal<typename SC_RemoveFnConstVolatileReference<T>::Type> {};

template <typename T> struct SC_IsMemberFunctionPointer { static constexpr const bool gValue = false; };
template <typename Function, typename Class> struct SC_IsMemberFunctionPointer<Function Class::*> : SC_IsFunction<Function> { typedef typename SC_RemoveFnConstVolatileReference<Function>::Type Signature; };
