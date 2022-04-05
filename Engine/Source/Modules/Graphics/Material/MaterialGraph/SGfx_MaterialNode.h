#pragma once

class SGfx_MaterialCompiler;
class SGfx_MaterialNode;
struct SGfx_MaterialNodePin;
struct SGfx_MaterialNodeInputPin;
struct SGfx_MaterialNodeOutputPin;

struct SGfx_MaterialNodeConnection
{
	SGfx_MaterialNodeConnection() : mSource(nullptr), mDestination(nullptr) {}
	SGfx_MaterialNodeConnection(SGfx_MaterialNodeOutputPin* aSource, SGfx_MaterialNodeInputPin* aDestination) : mSource(aSource), mDestination(aDestination) {}
	SGfx_MaterialNodeOutputPin* mSource;
	SGfx_MaterialNodeInputPin* mDestination;
};

struct SGfx_MaterialNodePin
{
	SGfx_MaterialNodePin(SGfx_MaterialNode* aParentNode) : mParentNode(aParentNode) {}
	virtual ~SGfx_MaterialNodePin() {}

	virtual bool IsConnected() const = 0;

	SGfx_MaterialNode* GetParent() const { return mParentNode; }

protected:
	SGfx_MaterialNode* mParentNode;
};

struct SGfx_MaterialNodeOutputPin : public SGfx_MaterialNodePin
{
	friend struct SGfx_MaterialNodeInputPin;
public:
	SGfx_MaterialNodeOutputPin(SGfx_MaterialNode* aParentNode) : SGfx_MaterialNodePin(aParentNode) {}
	~SGfx_MaterialNodeOutputPin()
	{
		if (mDataDestructor)
			mDataDestructor();
	}

	template<class T>
	inline void Write(const T& aSrc)
	{
		mData.Respace(sizeof(T));
		T* obj = new(static_cast<void*>(mData.GetBuffer())) T(aSrc);
		if (obj)
			mDataDestructor = [obj]() { obj->~T(); }; // To make sure we actually call the destructor of the underlying type before we release the memory.
	}

	bool IsConnected() const override
	{
		for (const SGfx_MaterialNodeConnection& connection : mConnections)
		{
			if (connection.mSource != nullptr)
				return true;
		}
		return false;
	}

	SC_Array<SGfx_MaterialNodeConnection> mConnections;

private:
	SC_Array<uint8> mData;
	std::function<void()> mDataDestructor;
};

struct SGfx_MaterialNodeInputPin : public SGfx_MaterialNodePin
{
public:
	SGfx_MaterialNodeInputPin(SGfx_MaterialNode* aParentNode) : SGfx_MaterialNodePin(aParentNode) {}
	~SGfx_MaterialNodeInputPin() {}

	template<class T>
	inline const T* Read() const
	{
		if (!mConnection.mSource)
			return nullptr;

		const SC_Array<uint8>& data = mConnection.mSource->mData;
		SC_ASSERT(data.GetByteSize() == sizeof(T), "Trying to read invalid type");

		return reinterpret_cast<const T*>(data.GetBuffer());
	}

	bool IsConnected() const override
	{
		return (mConnection.mSource != nullptr);
	}

	SGfx_MaterialNodeConnection mConnection;
};

class SGfx_MaterialNode
{
	friend class SGfx_MaterialNodeGraph;
public:
	SGfx_MaterialNode() : mNodeId(gInvalidNodeId) {}
	virtual ~SGfx_MaterialNode() {}
	virtual bool Compile(SGfx_MaterialCompiler* aCompiler) = 0;

	bool AddConnection(SGfx_MaterialNodeOutputPin* aSourcePin, uint32 aInputPinIndex);
	bool RemoveConnection(SGfx_MaterialNodeOutputPin* aSourcePin, uint32 aInputPinIndex);

	bool AddConnection(SGfx_MaterialNodeInputPin* aDestinationPin, uint32 aOutputPinIndex);
	bool RemoveConnection(SGfx_MaterialNodeInputPin* aDestinationPin, uint32 aOutputPinIndex);

	const SGfx_MaterialNodeInputPin* GetInputPin(uint32 aPinIndex) const { return &mInputs[aPinIndex]; }
	SGfx_MaterialNodeInputPin* GetInputPin(uint32 aPinIndex) { return &mInputs[aPinIndex]; }
	const SC_Array<SGfx_MaterialNodeInputPin>& GetInputs() const { return mInputs; }
	SC_Array<SGfx_MaterialNodeInputPin>& GetInputs() { return mInputs; }

	const SGfx_MaterialNodeOutputPin* GetOutputPin(uint32 aPinIndex) const { return &mOutputs[aPinIndex]; }
	SGfx_MaterialNodeOutputPin* GetOutputPin(uint32 aPinIndex) { return &mOutputs[aPinIndex]; }
	const SC_Array<SGfx_MaterialNodeOutputPin>& GetOutputs() const { return mOutputs; }
	SC_Array<SGfx_MaterialNodeOutputPin>& GetOutputs() { return mOutputs; }

	//#if IS_EDITOR_BUILD
	//	virtual void Draw() {}
	//#endif 

	uint32 GetId() const { return mNodeId; }

	static constexpr uint32 gInvalidNodeId = uint32(-1);

protected:
	SC_Array<SGfx_MaterialNodeInputPin> mInputs;
	SC_Array<SGfx_MaterialNodeOutputPin> mOutputs;

	uint32 mNodeId;
};