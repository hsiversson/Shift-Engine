#pragma once

enum SR_RootSignatureFlag : uint32
{
	SR_RootSignatureFlag_None = 0,
	SR_RootSignatureFlag_AllowInputAssembler = 0x1,
	SR_RootSignatureFlag_DenyVertexShaderAccess = 0x2,
	SR_RootSignatureFlag_DenyHullShaderAccess = 0x4,
	SR_RootSignatureFlag_DenyDomainShaderAccess = 0x8,
	SR_RootSignatureFlag_DenyGeometryShaderAccess = 0x10,
	SR_RootSignatureFlag_DenyPixelShaderAccess = 0x20,
	SR_RootSignatureFlag_DenyAmplificationShaderAccess = 0x40,
	SR_RootSignatureFlag_DenyMeshShaderAccess = 0x80,

	SR_RootSignatureFlag_DescriptorHeapDirectAccess = 0x100,
	SR_RootSignatureFlag_SamplerHeapDirectAccess = 0x200,

	SR_RootSignatureFlag_DefaultSetup = SR_RootSignatureFlag_AllowInputAssembler | SR_RootSignatureFlag_DenyHullShaderAccess | SR_RootSignatureFlag_DenyDomainShaderAccess | SR_RootSignatureFlag_DenyGeometryShaderAccess | SR_RootSignatureFlag_DenyAmplificationShaderAccess | SR_RootSignatureFlag_DenyMeshShaderAccess,
	SR_RootSignatureFlag_ComputeSetup = SR_RootSignatureFlag_DenyVertexShaderAccess | SR_RootSignatureFlag_DenyHullShaderAccess | SR_RootSignatureFlag_DenyDomainShaderAccess | SR_RootSignatureFlag_DenyGeometryShaderAccess | SR_RootSignatureFlag_DenyPixelShaderAccess | SR_RootSignatureFlag_DenyAmplificationShaderAccess | SR_RootSignatureFlag_DenyMeshShaderAccess,
	SR_RootSignatureFlag_MeshShaderSetup = SR_RootSignatureFlag_DenyVertexShaderAccess | SR_RootSignatureFlag_DenyHullShaderAccess | SR_RootSignatureFlag_DenyDomainShaderAccess | SR_RootSignatureFlag_DenyGeometryShaderAccess,
};

enum class SR_RootParamVisibility
{
	All,
	Vertex,
	Hull,
	Domain,
	Geometry,
	Pixel,
	Amplification,
	Mesh,
};

enum class SR_RootParamType
{
	Table,
	CBV,
	SRV,
	UAV,
};

struct SR_RootTableRange
{
	uint32 mStartRegister;
	uint32 mNumDescriptors;
	uint32 mRegisterSpace;
	SR_DescriptorType mDescriptorType;
};

struct SR_RootParam
{
public:
	void InitAsTable(SR_RootTableRange* aRanges, uint32 aNumRanges, const SR_RootParamVisibility& aVisibility)
	{
		mDescriptorTable.mDescriptorRanges.Add(aRanges, aNumRanges);
		mVisibility = aVisibility;
		mType = SR_RootParamType::Table;
	}

	void InitAsDescriptor(uint32 aRegisterIndex, uint32 aRegisterSpace, const SR_RootParamVisibility& aVisibility, const SR_RootParamType& aType)
	{
		mDescriptor.mRegisterIndex = aRegisterIndex;
		mDescriptor.mRegisterSpace = aRegisterSpace;
		mVisibility = aVisibility;
		mType = aType;
	}

	struct DescriptorTableInfo
	{
		SC_Array<SR_RootTableRange> mDescriptorRanges;
	} mDescriptorTable;

	struct DescriptorInfo
	{
		uint32 mRegisterIndex;
		uint32 mRegisterSpace;
	} mDescriptor;

	SR_RootParamVisibility mVisibility;
	SR_RootParamType mType;
};

struct SR_StaticSamplerProperties
{
	SR_SamplerProperties mProperties;
	SR_RootParamVisibility mVisibility;
};

struct SR_RootSignatureProperties
{
	SR_RootSignatureProperties() : mFlags(0), mIsCompute(false), mDebugName(nullptr) {}

	SC_Array<SR_RootParam> mRootParams;
	SC_Array<SR_StaticSamplerProperties> mStaticSamplers;
	uint32 mFlags;
	bool mIsCompute;

	const char* mDebugName;
};

class SR_RootSignature
{
public:
	SR_RootSignature(const SR_RootSignatureProperties& aProperties);
	virtual ~SR_RootSignature();

	const SR_RootSignatureProperties& GetProperties() const;

protected:
	SR_RootSignatureProperties mProperties;
};

