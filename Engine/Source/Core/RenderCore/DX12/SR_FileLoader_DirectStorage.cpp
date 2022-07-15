#include "SR_FileLoader_DirectStorage.h"

#if SR_ENABLE_DIRECTSTORAGE
#include "SR_RenderDevice_DX12.h"

#include "dstorage.h"

SR_FileLoader_DirectStorage::SR_FileLoader_DirectStorage()
{

}

SR_FileLoader_DirectStorage::~SR_FileLoader_DirectStorage()
{

}

bool SR_FileLoader_DirectStorage::Init()
{
	HRESULT hr = DStorageGetFactory(IID_PPV_ARGS(&mFactory));

	DSTORAGE_QUEUE_DESC queueDesc = {};
	queueDesc.Capacity = DSTORAGE_MAX_QUEUE_CAPACITY;
	queueDesc.Priority = DSTORAGE_PRIORITY_NORMAL;
	queueDesc.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
	queueDesc.Device = SR_RenderDevice_DX12::gInstance->GetD3D12Device();

	hr = mFactory->CreateQueue(&queueDesc, IID_PPV_ARGS(&mQueue[RequestType_File]));
	if (!VerifyHRESULT(hr))
		return false;

	queueDesc.SourceType = DSTORAGE_REQUEST_SOURCE_MEMORY;
	hr = mFactory->CreateQueue(&queueDesc, IID_PPV_ARGS(&mQueue[RequestType_Memory]));
	if (!VerifyHRESULT(hr))
		return false;

	return true;
}

bool SR_FileLoader_DirectStorage::LoadFromFile(const char* /*aPath*/)
{
	DSTORAGE_REQUEST request = {};
	request.Options.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
	return LoadInternal(request);
}

bool SR_FileLoader_DirectStorage::LoadFromMemory(const char* /*aPath*/)
{
	DSTORAGE_REQUEST request = {};
	request.Options.SourceType = DSTORAGE_REQUEST_SOURCE_MEMORY;
	return LoadInternal(request);
}

bool SR_FileLoader_DirectStorage::LoadInternal(DSTORAGE_REQUEST& aRequestDesc)
{
	IDStorageQueue* queue = aRequestDesc.Options.SourceType == DSTORAGE_REQUEST_SOURCE_FILE ? mQueue[RequestType_File].Get() : mQueue[RequestType_Memory].Get();

	aRequestDesc.Options.DestinationType = DSTORAGE_REQUEST_DESTINATION_BUFFER;
	//aRequestDesc.Source.File.Source = file.get();
	aRequestDesc.Source.File.Offset = 0;
	//aRequestDesc.Source.File.Size = fileSize;
	//aRequestDesc.UncompressedSize = fileSize;
	//aRequestDesc.Destination.Buffer.Resource = bufferResource.get();
	aRequestDesc.Destination.Buffer.Offset = 0;
	aRequestDesc.Destination.Buffer.Size = aRequestDesc.Source.File.Size;

	queue->EnqueueRequest(&aRequestDesc);

	ID3D12Fence* fence = nullptr;
	queue->EnqueueSignal(fence, 0);

	return true;
}
#endif