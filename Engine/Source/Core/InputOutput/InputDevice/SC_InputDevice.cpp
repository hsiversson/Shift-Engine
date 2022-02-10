#include "SC_InputDevice.h"

SC_InputDevice::SC_InputDevice()
	: mType(SC_InputDeviceType::Unknown)
{

}

SC_InputDevice::~SC_InputDevice()
{

}

const SC_InputDeviceType& SC_InputDevice::GetType() const
{
	return mType;
}

