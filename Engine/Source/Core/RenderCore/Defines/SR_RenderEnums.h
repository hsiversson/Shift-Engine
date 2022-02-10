#pragma once
#include "SR_RenderDefines.h"

enum class SR_API
{
	Unknown,
#if ENABLE_DX12
	D3D12,
#endif
#if ENABLE_VULKAN
	Vulkan,
#endif
};

enum class SR_ShaderModel
{
	Unknown,
	SM_5_1,
	SM_6_0,
	SM_6_1,
	SM_6_2,
	SM_6_3,
	SM_6_4,
	SM_6_5,
	SM_6_6,
	SM_6_7,
};

enum class SR_ShaderType
{
	Vertex,
#if ENABLE_MESH_SHADERS
	Mesh,
	Amplification,
#endif
	Pixel,
	Compute,
#if ENABLE_RAYTRACING
	Raytracing,
#endif
	COUNT
};

enum class SR_RootSignatureType 
{ 
	Graphics, 
	GraphicsMS, 
	Compute,
#if ENABLE_RAYTRACING
	Raytracing,
#endif
	COUNT
};

enum class SR_DescriptorType
{
	SRV,
	CBV,
	UAV,
	Sampler
};

enum class SR_StencilOperator
{
	Keep,
	Zero,
	Replace,
	IncrementSaturate,
	DecrementSaturate,
	Invert,
	Increment,
	Decrement,
	COUNT,
};

enum class SR_ComparisonFunc
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always,
	COUNT,
};

enum class SR_CullMode
{
	None,
	Front,
	Back,
	COUNT,
};

enum class SR_BlendFunc
{
	Add,
	Subtract,
	ReverseSubtract,
	Min,
	Max,
	COUNT,
};

enum class SR_BlendMode
{
	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstColor,
	OneMinusDstColor,
	DstAlpha,
	OneMinusDstAlpha,
	SrcAlphaSaturate,
	BlendFactor,
	OneMinusBlendFactor,
	Src1Color,
	OneMinusSrc1Color,
	Src1Alpha,
	OneMinusSrc1Alpha,
	COUNT,
};

enum class SR_ColorWriteMask
{
	None = (0),
	Red = (1 << 0),
	Green = (1 << 1),
	Blue = (1 << 2),
	Alpha = (1 << 3),
	RGB = (Red | Green | Blue),
	RGBA = (Red | Green | Blue | Alpha),
};

enum class SR_PrimitiveTopology
{
	TriangleList,
	LineList,
	LineStrip,
	PointList,
};

enum class SR_WrapMode
{
	Wrap = 0,
	Clamp,
	Mirror,
	MirrorOnce,
	Border,
	COUNT
};

enum class SR_FilterMode
{
	Point = 0,
	Linear,
	Min,
	Max,
	COUNT
};

enum class SR_ResourceType
{
	Unknown,
	Texture1D,
	Texture2D,
	Texture3D,
	Buffer
};

enum SR_ResourceState : unsigned int
{
	SR_ResourceState_Common = 0,
	SR_ResourceState_Present = SR_ResourceState_Common,
	SR_ResourceState_Read,
	SR_ResourceState_UnorderedAccess,
	SR_ResourceState_GraphicsWrite,
	SR_ResourceState_ComputeWrite,
	SR_ResourceState_Indirect,
	SR_ResourceState_Idle,
	SR_ResourceState_Readback,
	SR_ResourceState_RenderTarget,
	SR_ResourceState_CopyDst,
	SR_ResourceState_CopySrc,
	SR_ResourceState_DepthRead,
	SR_ResourceState_DepthWrite,
	SR_ResourceState_AccelerationStructure,
};

enum SR_BufferBindFlag : unsigned int 
{
	SR_BufferBindFlag_None				= 0,
	SR_BufferBindFlag_Staging			= 0x1,
	SR_BufferBindFlag_VertexBuffer		= 0x2,
	SR_BufferBindFlag_IndexBuffer		= 0x4,
	SR_BufferBindFlag_ConstantBuffer	= 0x8,
	SR_BufferBindFlag_Buffer			= 0x10,
	SR_BufferBindFlag_IndirectBuffer	= 0x20,
	SR_BufferBindFlag_RaytracingBuffer	= 0x40,
	SR_BufferBindFlag_Readback			= 0x80,
	SR_BufferBindFlag_COUNT				= 6,
};

enum class SR_ShadingRate
{
	VRS_1X1,
	VRS_1X2,
	VRS_2X1,
	VRS_2X2,
};

enum SR_TextureChannelOrder
{
	SR_CHANNELORDER_NONE = 0,
	SR_CHANNELORDER_RGBA = 0x40123,
	SR_CHANNELORDER_ARGB = 0x41230,
	SR_CHANNELORDER_BGRA = 0x42103,
	SR_CHANNELORDER_ABGR = 0x43210,

	SR_CHANNELORDER_NATIVE_ARGB = SR_CHANNELORDER_BGRA, // matches 0xAARRGGBB for uint8, 4 channels

	SR_CHANNELORDER_RGB = 0x3012f,
	SR_CHANNELORDER_BGR = 0x3210f,

	SR_CHANNELORDER_RG = 0x201ff,

	SR_CHANNELORDER_RA = 0x20ff1,

	SR_CHANNELORDER_R = 0x10fff,
};