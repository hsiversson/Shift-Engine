#include "SR_Buffer_DX12.h"

#if SR_ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_BufferResource_DX12.h"

SR_Buffer_DX12::SR_Buffer_DX12(const SR_BufferProperties& aProperties, const SC_Ref<SR_BufferResource>& aResource)
	: SR_Buffer(aProperties, aResource)
{
	mDX12Resource = static_cast<SR_BufferResource_DX12*>(GetResource());
}

SR_Buffer_DX12::~SR_Buffer_DX12()
{

}

bool SR_Buffer_DX12::Init()
{
	const SR_BufferResourceProperties& bufferResourceProperties = mResource->GetProperties();

	if (mProperties.mWritable)
	{
		SC_ASSERT(bufferResourceProperties.mWritable, "Resource doesn't allow write access.");
		return InitAsUAV();
	}
	else
		return InitAsSRV();
}

D3D12_CPU_DESCRIPTOR_HANDLE SR_Buffer_DX12::GetCPUHandle() const
{
	return D3D12_CPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleCPU };
}

D3D12_GPU_DESCRIPTOR_HANDLE SR_Buffer_DX12::GetGPUHandle() const
{
	return D3D12_GPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleGPU };
}

bool SR_Buffer_DX12::InitAsSRV()
{
	const SR_BufferResourceProperties& bufferResourceProperties = mResource->GetProperties();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

	ID3D12Resource* resource = nullptr;
	if (mProperties.mType == SR_BufferType::Bytes)
	{
		//SC_ASSERT(!(mProperties.mFirstElement & 3));
		//SC_ASSERT(!(mProperties.mElementCount & 3));

		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
		srvDesc.Buffer.FirstElement = mProperties.mFirstElement / 4;
		srvDesc.Buffer.NumElements = mProperties.mElementCount / 4;

		resource = mDX12Resource->GetD3D12Resource();
	}
	else if (mProperties.mType == SR_BufferType::RaytracingBuffer)
	{
		SC_ASSERT(bufferResourceProperties.mBindFlags & SR_BufferBindFlag_RaytracingBuffer);
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		srvDesc.RaytracingAccelerationStructure.Location = mDX12Resource->GetD3D12Resource()->GetGPUVirtualAddress();
	}
	else
	{
		if (mProperties.mType == SR_BufferType::Structured)
		{
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.Buffer.StructureByteStride = bufferResourceProperties.mElementSize;
		}
		else
			srvDesc.Format = SR_D3D12ConvertFormat(mProperties.mFormat);

		srvDesc.Buffer.NumElements = mProperties.mElementCount;
		srvDesc.Buffer.FirstElement = mProperties.mFirstElement;

		resource = mDX12Resource->GetD3D12Resource();
	}

	mDescriptor = SR_RenderDevice_DX12::gInstance->GetDefaultDescriptorHeap()->Alloc();
	SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreateShaderResourceView(resource, &srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleCPU });

	return true;
}

bool SR_Buffer_DX12::InitAsUAV()
{
	const SR_BufferResourceProperties& bufferResourceProperties = mResource->GetProperties();
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

	if (mProperties.mType == SR_BufferType::Bytes)
	{
		SC_ASSERT(!(mProperties.mFirstElement & 3));
		SC_ASSERT(!(mProperties.mElementCount & 3));

		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
		uavDesc.Buffer.FirstElement = mProperties.mFirstElement / 4;
		uavDesc.Buffer.NumElements = mProperties.mElementCount / 4;
	}
	else
	{
		if (mProperties.mType == SR_BufferType::Structured)
		{
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.Buffer.StructureByteStride = bufferResourceProperties.mElementSize;
		}
		else
			uavDesc.Format = SR_D3D12ConvertFormat(mProperties.mFormat);

		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		uavDesc.Buffer.FirstElement = mProperties.mFirstElement;
		uavDesc.Buffer.NumElements = mProperties.mElementCount;
	}

	mDescriptor = SR_RenderDevice_DX12::gInstance->GetDefaultDescriptorHeap()->Alloc();
	SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreateUnorderedAccessView(mDX12Resource->GetD3D12Resource(), nullptr, &uavDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleCPU });

	return true;
}

#endif