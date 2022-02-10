#pragma once
#include "Json.hpp"
#include <fstream>
#include <iomanip>

using SC_Json = nlohmann::json;

inline bool SC_LoadJson(const SC_FilePath& aFilePath, SC_Json& aOutData)
{
	std::ifstream stream(aFilePath.GetAbsolutePath());

	if (!stream.is_open())
		return false;

	stream >> aOutData;
	return true;
}

inline bool SC_SaveJson(const SC_FilePath& aFilePath, const SC_Json& aData)
{
	SC_FilePath::CreateDirectory(aFilePath.GetParentDirectory());
	std::ofstream stream(aFilePath.GetAbsolutePath());

	if (!stream.is_open())
		return false;

	stream << std::setw(4) << aData << std::endl;
	return true;
}