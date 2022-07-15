#pragma once

enum class SC_VirtualMachineBaseTypes_SIMD
{
	Bool,
	Float,
	Int,

	COUNT
};

enum class SC_VirtualMachineOp_SIMD
{
	done,
	add,
	sub,
	mul,
	div,
	mad,
	pow,
	lerp,
	rcp,
	sqrt,
	rsq,
	neg,
	abs,
	exp,
	exp2,
	log,
	log2,
	sin,
	asin,
	cos,
	acos,
	tan,
	atan,
	atan2,
	ceil,
	floor,
	round,
	frac,
	fmod,
	trunc,
	clamp,
	min,
	max,
	sign,
	step,
	rand,
	noise,

	cmplt,
	cmple,
	cmpgt,
	cmpge,
	cmpeq,
	cmpneq,
	select,

	iadd,
	isub,
	imul,
	idiv,
	iclamp,
	imin,
	imax,
	iabs,
	ineg,
	isign,
	irand,

	icmplt,
	icmple,
	icmpgt,
	icmpge,
	icmpeq,
	icmpneq,

	bit_and,
	bit_not,
	bit_or,
	bit_xor,
	bit_lshft,
	bit_rshft,

	logic_and,
	logic_or,
	logic_xor,
	logic_not,

	f2i,
	i2f,
	f2b,
	b2f,
	i2b,
	b2i,

	read_f32,
	read_noadvance_f32,
	read_f16,
	read_noadvance_f16,
	read_i32,
	read_noadvance_i32,

	write_f32,
	write_i32,
	write_f16,
	acquire_index,

	ext_func_call,

	exec_index,

	noise2d,
	noise3d,

	update_id,
	acquire_id,

	COUNT
};