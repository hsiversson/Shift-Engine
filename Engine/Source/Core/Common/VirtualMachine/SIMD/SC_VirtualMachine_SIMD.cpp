#include "SC_VirtualMachine_SIMD.h"
#include "SC_VirtualMachineContext_SIMD.h"

namespace SC_VirtualMachine_SIMD_Private
{
	//using ExecFunction = void(SR_ShaderVMContext&);
	typedef void(*ExecFunction)(SC_VirtualMachineContext_SIMD&);

	// Constants
	struct ConstantHandleBase
	{
		ConstantHandleBase(SC_VirtualMachineContext_SIMD& aContext)
			: mIndex(aContext.DecodeU16())
		{}

		void Advance() {}
		
		static void Optimize() {}
		static void OptimizeSkip() {}

		uint16 mIndex;
	};

	template<typename T>
	struct ConstantHandle : public ConstantHandleBase
	{
		ConstantHandle(SC_VirtualMachineContext_SIMD& aContext)
			: ConstantHandleBase(aContext)
			, mValue(*aContext.GetConstant<T>(mIndex))
		{}

		const T& Get() const { return mValue; }
		const T& GetAndAdvance() const { return mValue; }

		const T mValue;
	};

	// Registers
	struct RegisterHandleBase
	{
		RegisterHandleBase(SC_VirtualMachineContext_SIMD& aContext)
			: mIndex(aContext.DecodeU16())
		{}

		static void Optimize() {}

		uint16 mIndex;
	};

	template<typename T>
	struct RegisterHandle : public RegisterHandleBase
	{
	public:
		RegisterHandle(SC_VirtualMachineContext_SIMD& aContext)
			: RegisterHandleBase(aContext)
			, mRegister((T*)aContext.GetTempRegister(mIndex))
		{}

		const T Get() { return *mRegister; }
		T* GetDest() { return mRegister; }
		void Advance() { ++mRegister; }
		const T GetAndAdvance() { return *mRegister++; }
		T* GetDestAndAdvance() { return mRegister++; }
	private:
		T* mRegister;
	};

	// Kernels
	template<typename Kernel, typename DestinationHandle, typename Arg0Handle, uint32 InstancesPerOp>
	struct KernelHandle_Unary
	{

		static void Execute(SC_VirtualMachineContext_SIMD& aContext)
		{
			Arg0Handle arg0(aContext);
			DestinationHandle dst(aContext);

			uint32 numLoops;
			//for (uint32 i = 0; i < numLoops; ++i)
			//	Kernel::HandleKernel(aContext, dst.GetDestinationAndAdvance(), arg0.GetAndAdvance());
		}
	};

	template<typename Kernel, typename DestinationHandle, typename Arg0Handle, typename Arg1Handle, uint32 InstancesPerOp>
	struct KernelHandle_Binary
	{

		static void Execute(SC_VirtualMachineContext_SIMD& aContext)
		{
			Arg0Handle arg0(aContext);
			Arg1Handle arg1(aContext);
			DestinationHandle dst(aContext);

			uint32 numLoops;
			//for (uint32 i = 0; i < numLoops; ++i)
			//	Kernel::HandleKernel(aContext, dst.GetDestinationAndAdvance(), arg0.GetAndAdvance(), arg1.GetAndAdvance());
		}
	};

	template<typename Kernel, typename DestinationHandle, typename Arg0Handle, typename Arg1Handle, typename Arg2Handle, uint32 InstancesPerOp>
	struct KernelHandle_Trinary
	{

		static void Execute(SC_VirtualMachineContext_SIMD& aContext)
		{
			Arg0Handle arg0(aContext);
			Arg1Handle arg1(aContext);
			Arg2Handle arg2(aContext);
			DestinationHandle dst(aContext);

			uint32 numLoops;
			//for (uint32 i = 0; i < numLoops; ++i)
			//	Kernel::HandleKernel(aContext, dst.GetDestinationAndAdvance(), arg0.GetAndAdvance(), arg1.GetAndAdvance(), arg2.GetAndAdvance());
		}
	};
}

void SC_VirtualMachine_SIMD::Execute(ExecuteArgs& aArgs)
{
	const uint32 chunksPerBatch = 0;
	static constexpr uint32 gParallellVMInstancesPerChunk = 0;

	SC_VirtualMachine_SIMD_Private::ExecFunction optimizedJumpTable[10];

	auto ExecuteBatch = [&](uint32 aBatchIndex)
	{
		SC_VirtualMachineContext_SIMD& context = SC_VirtualMachineContext_SIMD::Get();
		context.Prepare();

		uint32 chunkIndex = aBatchIndex * chunksPerBatch;
		const uint32 firstInstance = chunkIndex * gParallellVMInstancesPerChunk;
		const uint32 lastInstance = SC_Min(aArgs.mInstanceCount, firstInstance + (chunksPerBatch * gParallellVMInstancesPerChunk));

		uint32 instancesRemaining = lastInstance - firstInstance;
		while (instancesRemaining > 0)
		{
			bool useOptimizedPath = false;
			if (useOptimizedPath)
			{
				context.PrepareChunk();

				for (;;)
				{
					if (SC_VirtualMachine_SIMD_Private::ExecFunction execFunc = optimizedJumpTable[0])
						execFunc(context);
					else
						break;
				}
			}
			else
			{
				context.PrepareChunk();

				SC_VirtualMachineOp_SIMD op = SC_VirtualMachineOp_SIMD::done;
				do 
				{
					switch (op)
					{
					case SC_VirtualMachineOp_SIMD::add: break;
					case SC_VirtualMachineOp_SIMD::sub: break;
					case SC_VirtualMachineOp_SIMD::mul: break;
					case SC_VirtualMachineOp_SIMD::div: break;
					case SC_VirtualMachineOp_SIMD::mad: break;
					case SC_VirtualMachineOp_SIMD::pow: break;
					case SC_VirtualMachineOp_SIMD::lerp: break;
					case SC_VirtualMachineOp_SIMD::rcp: break;
					case SC_VirtualMachineOp_SIMD::sqrt: break;
					case SC_VirtualMachineOp_SIMD::rsq: break;
					case SC_VirtualMachineOp_SIMD::neg: break;
					case SC_VirtualMachineOp_SIMD::abs: break;
					case SC_VirtualMachineOp_SIMD::exp: break;
					case SC_VirtualMachineOp_SIMD::exp2: break;
					case SC_VirtualMachineOp_SIMD::log: break;
					case SC_VirtualMachineOp_SIMD::log2: break;
					case SC_VirtualMachineOp_SIMD::sin: break;
					case SC_VirtualMachineOp_SIMD::asin: break;
					case SC_VirtualMachineOp_SIMD::cos: break;
					case SC_VirtualMachineOp_SIMD::acos: break;
					case SC_VirtualMachineOp_SIMD::tan: break;
					case SC_VirtualMachineOp_SIMD::atan: break;
					case SC_VirtualMachineOp_SIMD::atan2: break;
					case SC_VirtualMachineOp_SIMD::ceil: break;
					case SC_VirtualMachineOp_SIMD::floor: break;
					case SC_VirtualMachineOp_SIMD::round: break;
					case SC_VirtualMachineOp_SIMD::frac: break;
					case SC_VirtualMachineOp_SIMD::fmod: break;
					case SC_VirtualMachineOp_SIMD::trunc: break;
					case SC_VirtualMachineOp_SIMD::clamp: break;
					case SC_VirtualMachineOp_SIMD::min: break;
					case SC_VirtualMachineOp_SIMD::max: break;
					case SC_VirtualMachineOp_SIMD::sign: break;
					case SC_VirtualMachineOp_SIMD::step: break;
					case SC_VirtualMachineOp_SIMD::rand: break;
					case SC_VirtualMachineOp_SIMD::noise: break;
					case SC_VirtualMachineOp_SIMD::cmplt: break;
					case SC_VirtualMachineOp_SIMD::cmple: break;
					case SC_VirtualMachineOp_SIMD::cmpgt: break;
					case SC_VirtualMachineOp_SIMD::cmpge: break;
					case SC_VirtualMachineOp_SIMD::cmpeq: break;
					case SC_VirtualMachineOp_SIMD::cmpneq: break;
					case SC_VirtualMachineOp_SIMD::select: break;
					case SC_VirtualMachineOp_SIMD::iadd: break;
					case SC_VirtualMachineOp_SIMD::isub: break;
					case SC_VirtualMachineOp_SIMD::imul: break;
					case SC_VirtualMachineOp_SIMD::idiv: break;
					case SC_VirtualMachineOp_SIMD::iclamp: break;
					case SC_VirtualMachineOp_SIMD::imin: break;
					case SC_VirtualMachineOp_SIMD::imax: break;
					case SC_VirtualMachineOp_SIMD::iabs: break;
					case SC_VirtualMachineOp_SIMD::ineg: break;
					case SC_VirtualMachineOp_SIMD::isign: break;
					case SC_VirtualMachineOp_SIMD::irand: break;
					case SC_VirtualMachineOp_SIMD::icmplt: break;
					case SC_VirtualMachineOp_SIMD::icmple: break;
					case SC_VirtualMachineOp_SIMD::icmpgt: break;
					case SC_VirtualMachineOp_SIMD::icmpge: break;
					case SC_VirtualMachineOp_SIMD::icmpeq: break;
					case SC_VirtualMachineOp_SIMD::icmpneq: break;
					case SC_VirtualMachineOp_SIMD::bit_and: break;
					case SC_VirtualMachineOp_SIMD::bit_not: break;
					case SC_VirtualMachineOp_SIMD::bit_or: break;
					case SC_VirtualMachineOp_SIMD::bit_xor: break;
					case SC_VirtualMachineOp_SIMD::bit_lshft: break;
					case SC_VirtualMachineOp_SIMD::bit_rshft: break;
					case SC_VirtualMachineOp_SIMD::logic_and: break;
					case SC_VirtualMachineOp_SIMD::logic_or: break;
					case SC_VirtualMachineOp_SIMD::logic_xor: break;
					case SC_VirtualMachineOp_SIMD::logic_not: break;
					case SC_VirtualMachineOp_SIMD::f2i: break;
					case SC_VirtualMachineOp_SIMD::i2f: break;
					case SC_VirtualMachineOp_SIMD::f2b: break;
					case SC_VirtualMachineOp_SIMD::b2f: break;
					case SC_VirtualMachineOp_SIMD::i2b: break;
					case SC_VirtualMachineOp_SIMD::b2i: break;
					case SC_VirtualMachineOp_SIMD::read_f32: break;
					case SC_VirtualMachineOp_SIMD::read_noadvance_f32: break;
					case SC_VirtualMachineOp_SIMD::read_f16: break;
					case SC_VirtualMachineOp_SIMD::read_noadvance_f16: break;
					case SC_VirtualMachineOp_SIMD::read_i32: break;
					case SC_VirtualMachineOp_SIMD::read_noadvance_i32: break;
					case SC_VirtualMachineOp_SIMD::write_f32: break;
					case SC_VirtualMachineOp_SIMD::write_i32: break;
					case SC_VirtualMachineOp_SIMD::write_f16: break;
					case SC_VirtualMachineOp_SIMD::acquire_index: break;
					case SC_VirtualMachineOp_SIMD::ext_func_call: break;
					case SC_VirtualMachineOp_SIMD::exec_index: break;
					case SC_VirtualMachineOp_SIMD::noise2d: break;
					case SC_VirtualMachineOp_SIMD::noise3d: break;
					case SC_VirtualMachineOp_SIMD::update_id: break;
					case SC_VirtualMachineOp_SIMD::acquire_id: break;
					case SC_VirtualMachineOp_SIMD::done: break;
					default:
						SC_ASSERT(false, "Unknown op: {}.", (uint32)op);
						return;
					}

				} while (op != SC_VirtualMachineOp_SIMD::done);
			}

			instancesRemaining -= gParallellVMInstancesPerChunk;
			++chunkIndex;
		}

		context.Finish();
	};

	bool parallelize = false;
	if (parallelize)
		ExecuteBatch(0);
	else
		ExecuteBatch(0);
}
