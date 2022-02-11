#pragma once
#include "InputOutput/KeyCodes/SC_KeyCodes.h"

enum class SC_MessageType
{
	Key = 0,
	InputChar,
	Mouse,
	MousePos,
	MouseMickeys,
	Scroll,
	Window,
	Unknown,
};

enum class SC_InputKeyStateMessage
{
	Pressed = 0,
	Released,
	Unknown,
};

struct SC_KeyMessageData
{
	SC_InputKeyStateMessage mState = SC_InputKeyStateMessage::Unknown;
	SC_KeyCode mKey;
}; 
struct SC_MouseKeyMessageData
{
	SC_InputKeyStateMessage mState = SC_InputKeyStateMessage::Unknown;
	SC_MouseKeyCode mKey;
};

class SC_Message
{
public:
	SC_Message()
		: mType(SC_MessageType::Unknown)
		, mStorage{}
	{
		SC_Fill(mStorage, gInternalBlockSize, 0);
	}

	SC_Message(const SC_Message& aOther)
		: mType(SC_MessageType::Unknown)
		, mStorage{}
	{
		SC_Memcpy(this, &aOther, sizeof(SC_Message));
	}

	virtual ~SC_Message() 
	{
	}


	template <class T>
	const T& Get() const
	{
		static_assert(!(gInternalBlockSize < sizeof(T)), "T cannot be contained within, storage not large enough");
		return *reinterpret_cast<const T*>(&mStorage[0]);
	}

	template <class T>
	void Store(const T& aInData)
	{
		static_assert(SC_IsTriviallyCopyableValue<T> && SC_IsTriviallyDestructibleValue<T>	, "No complex types here please");
		static_assert(!(gInternalBlockSize < sizeof(T))										, "T cannot be contained within, storage not large enough");
		static_assert(!(gInternalBlockAlign < alignof(T))									, "Alignment of T is larger than that of storage");

		*((T*)&mStorage[0]) = aInData;
	}

	SC_MessageType mType;

private:
	static constexpr uint8 gInternalBlockAlign = alignof(SC_MaxAlignType);
	static constexpr uint8 gInternalBlockSize = gInternalBlockAlign * 2;

	alignas(gInternalBlockAlign) uint8 mStorage[gInternalBlockSize];
};