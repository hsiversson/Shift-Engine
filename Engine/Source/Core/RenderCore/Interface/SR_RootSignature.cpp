#include "SR_RootSignature.h"

SR_RootSignature::SR_RootSignature(const SR_RootSignatureProperties& aProperties)
	: mProperties(aProperties)
{

}

SR_RootSignature::~SR_RootSignature()
{

}

const SR_RootSignatureProperties& SR_RootSignature::GetProperties() const
{
	return mProperties;
}
