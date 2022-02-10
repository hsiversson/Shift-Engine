#pragma once
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