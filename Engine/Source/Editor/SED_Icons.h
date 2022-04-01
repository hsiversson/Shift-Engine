#pragma once

class SR_Texture;
class SED_Icons
{
public:
	static void Create();
	static void Destroy();
	static SED_Icons* Get();

public:
	enum class IconType
	{
		Visible,
		NonVisible,

		COUNT
	};

public:
	SED_Icons();
	~SED_Icons();

	SR_Texture* GetIconByType(const IconType& aType) const;

private:
	SC_Ref<SR_Texture> mIconsByType[static_cast<uint32>(IconType::COUNT)];

	static SED_Icons* gInstance;
};

