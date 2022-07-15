#pragma once

enum class SED_AssetType
{
	Unknown,
	Mesh,
	Model,
	Texture,
	Material,
	Script,
	Font,
	DataTable,
	Level,
	Project,
	COUNT
};

class SED_Asset
{
public:
	SED_Asset();
	~SED_Asset();

	const SED_AssetType& GetType() const;

private:
	SED_AssetType mType;
};

