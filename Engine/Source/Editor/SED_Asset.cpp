#include "SED_Asset.h"

SED_Asset::SED_Asset()
	: mType(SED_AssetType::Unknown)
{

}

SED_Asset::~SED_Asset()
{

}

const SED_AssetType& SED_Asset::GetType() const
{
	return mType;
}
