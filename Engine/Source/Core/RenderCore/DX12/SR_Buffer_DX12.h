
#pragma once

#if SR_ENABLE_DX12

class SR_BufferResource_DX12;
class SR_Buffer_DX12 : public SR_Buffer
{
public:
	SR_Buffer_DX12(const SR_BufferProperties& aProperties, const SC_Ref<SR_BufferResource>& aResource);
	~SR_Buffer_DX12();

	bool Init();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;

private:
	bool InitAsSRV();
	bool InitAsUAV();

	SR_BufferResource_DX12* mDX12Resource;
};

#endif
