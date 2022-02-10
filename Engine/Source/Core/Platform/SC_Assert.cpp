
#include "SC_Assert.h"

bool SC_Assert(const char* /*aFile*/, int /*aLine*/, const char* /*aString*/)
{
	return true;
}

bool SC_Assert(const char* /*aFile*/, int /*aLine*/, const char* /*aString*/, const char* /*aMessageFormatString*/, ...)
{
	return true;
}
