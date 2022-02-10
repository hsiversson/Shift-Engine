#pragma once
#include <map>
#include <vector>
#include <string>

#define ENABLE_COMMAND_LINE (1)

class SC_CommandLine
{
public:
	static bool Init(char** aArgsV, int aArgsC);
	static void Destroy();
	static bool HasCommand(const char* aCommand);
	static bool HasArgument(const char* aCommand, const char* aArg);

private:
	SC_CommandLine(char** aArgsV, int aArgsC);
	~SC_CommandLine();

#if ENABLE_COMMAND_LINE
	static SC_CommandLine* gInstance;

	std::map<std::string, std::vector<std::string>> mCommands;
#endif
};

