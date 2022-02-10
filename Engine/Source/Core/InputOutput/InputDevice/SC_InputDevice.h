#pragma once

enum class SC_InputDeviceType
{
	Keyboard,
	Mouse,
	Gamepad,
	COUNT,
	Unknown = COUNT,
};

class SC_InputDevice
{
public:
	SC_InputDevice();
	virtual ~SC_InputDevice();

	template<class InputDeviceType>
	InputDeviceType& As()
	{
		return *(static_cast<InputDeviceType*>(this));
	}

	const SC_InputDeviceType& GetType() const;


protected:
	SC_InputDeviceType mType;
};