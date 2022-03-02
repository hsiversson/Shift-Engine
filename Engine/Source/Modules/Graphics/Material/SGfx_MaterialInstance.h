#pragma once
#include "SGfx_Material.h"

/*
*
*	A reference to an existing material template with options to override resources and settings
* 
*/

struct SR_MaterialInstanceProperties
{

};

template<class T>
struct SGfx_MaterialInstanceParameter
{
public:
	SGfx_MaterialInstanceParameter(const T& aType, SGfx_MaterialParameter<T>* aParentParam)
		: mParentParameter(aParentParam)
		, mIsOverridden(false)
		, mType(aType)
	{

	}

	const T& Get() const
	{
		if (mIsOverridden)
			return mParameter;
		else
			return mParentParameter->Get();
	}

	void Override(const T& aParam)
	{
		mParameter = aParam;
		mIsOverridden = true;
	}

	void RemoveOverride()
	{
		mIsOverridden = false;
	}

public:
	enum class Type
	{
		Constant,
		Constant2,
		Constant3,
		Constant4,
		Texture
	};

private:
	T mParameter;
	SGfx_MaterialParameter<T>* mParentParameter;
	bool mIsOverridden;
	Type mType;
};

class SGfx_MaterialInstance
{
public:
	static SC_Ref<SGfx_MaterialInstance> GetDefault();
	static SC_Ref<SGfx_MaterialInstance> Create(const SC_Ref<SGfx_Material>& aMaterialTemplate);
public:
	SGfx_MaterialInstance(const SC_Ref<SGfx_Material>& aMaterialTemplate);
	~SGfx_MaterialInstance();

	SGfx_Material* GetMaterialTemplate() const;
	uint32 GetMaterialIndex() const;

	void OverrideTexture(const SC_Ref<SR_Texture>& aTexture, uint32 aIndex);

private:
	void UpdateGPUData();

	//SC_Array<SGfx_MaterialInstanceParameter<SC_Ref<SR_Texture>>> mTextureOverrides;

	SC_Array<SC_Ref<SR_Texture>> mTextureOverrides;

	SC_Ref<SGfx_Material> mMaterialTemplate;

	uint32 mMaterialIndex;
};

/*

	MATERIAL INSTANCE JSON TEMPLATE
	{
		"MaterialTemplate": "../Materials/MaterialTemplate.smt,
		"PropertyOverrides": 
		[
			"Texture_0": "../Textures/black.dds",
			"Constant_0": 2.0f,
			"Constant3_0": [1.0f, 0.0f, 1.0f]
		],
	}
*/