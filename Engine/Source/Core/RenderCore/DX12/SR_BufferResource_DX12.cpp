
#include "SR_BufferResource_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"

SR_BufferResource_DX12::SR_BufferResource_DX12(const SR_BufferResourceProperties& aProperties)
	: SR_BufferResource(aProperties)
	, mD3D12Resource(nullptr)
{

}

SR_BufferResource_DX12::~SR_BufferResource_DX12()
{
	if (mD3D12Resource)
	{
		mD3D12Resource->Release();
		mD3D12Resource = nullptr;
	}
}

bool SR_BufferResource_DX12::Init(const void* aInitialData)
{
	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = uint64(mProperties.mElementCount) * uint64(mProperties.mElementSize);
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (mProperties.mWritable)
		bufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

	if (mProperties.mBindFlags & SR_BufferBindFlag_ConstantBuffer)
	{
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
		bufferDesc.Width = SC_Align(bufferDesc.Width, 256);
	}

	if (mProperties.mBindFlags & SR_BufferBindFlag_Readback)
	{
		heapProps.Type = D3D12_HEAP_TYPE_READBACK;
		initialState = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	if (mProperties.mBindFlags & SR_BufferBindFlag_RaytracingBuffer)
		initialState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

	if (mProperties.mBindFlags == SR_BufferBindFlag_Buffer && mProperties.mWritable)
		initialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	if (aInitialData)
		initialState = D3D12_RESOURCE_STATE_COPY_DEST;

	if (mProperties.mIsUploadBuffer)
	{
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	HRESULT hr = SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, initialState, nullptr, IID_PPV_ARGS(&mD3D12Resource));
	if (FAILED(hr))
	{
		//LOG_ERROR("Could not create buffer with id: %ls \n", aDebugName);
		return false;
	}
	mTrackedD3D12Resource = mD3D12Resource;

	if (heapProps.Type == D3D12_HEAP_TYPE_UPLOAD)
	{
		D3D12_RANGE readRange{ 0, 0 };
		void* dataPtr = nullptr;
		hr = mD3D12Resource->Map(0, &readRange, &dataPtr);
		if (FAILED(hr))
			assert(false && "Could not map resource.");

		mDataPtr = (uint8*)dataPtr;
	}

	if (aInitialData)
	{
		SR_BufferResourceProperties uploadProps(mProperties);
		uploadProps.mIsUploadBuffer = true;

		SR_BufferResource_DX12* uploadBuffer = new SR_BufferResource_DX12(uploadProps);
		if (!uploadBuffer->Init(nullptr))
		{
			assert(false && "Could not init upload buffer.");
		}

		uploadBuffer->UpdateData(0, aInitialData, bufferDesc.Width);

		SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->CreateCommandList(SR_CommandListType::Copy);
		cmdList->Begin();
		cmdList->CopyResource(this, uploadBuffer);
		cmdList->End();

		SR_Fence fence = SR_RenderDevice::gInstance->GetCommandQueue(SR_CommandListType::Copy)->SubmitCommandList(cmdList.get());
		SR_RenderDevice::gInstance->WaitForFence(fence);
		delete uploadBuffer;
	}

	if (mProperties.mDebugName)
	{
		mD3D12Resource->SetName(SC_UTF8ToUTF16(mProperties.mDebugName).c_str());
	}

	return true;
}

uint64 SR_BufferResource_DX12::GetGPUAddressStart() const
{
	return mD3D12Resource->GetGPUVirtualAddress();
}

ID3D12Resource* SR_BufferResource_DX12::GetD3D12Resource() const
{
	return mD3D12Resource;
}

#endif