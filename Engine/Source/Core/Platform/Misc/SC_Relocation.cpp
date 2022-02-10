#include "SC_Relocation.h"
#include "Misc/SC_MemoryFunctions.h"

enum	SFaultyTestEnum { i = 1 };
struct	SFaultyTestData { int32 i; };
struct	SFaultyTestStruct { SFaultyTestStruct() : i(0) {} void operator=(const SFaultyTestStruct&) {} int32 i; };
class	SFaultyTestClass { public: SFaultyTestClass() : i(0) {} void operator=(const SFaultyTestClass&) {} int32 i; };
union	SFaultyTestUnion { int i; int j; };
struct	SFaultyTestNotAllowed { ~SFaultyTestNotAllowed() { volatile int i = 1; i = 2; (void)i; } };

static_assert(SC_AllowMemcpyRelocation<int>::mValue, "[int] may NOT be relocated by memcpy");
static_assert(SC_AllowMemcpyRelocation<float>::mValue, "[float] may NOT be relocated by memcpy");
static_assert(SC_AllowMemcpyRelocation<void*>::mValue, "[pointers] may NOT be relocated by memcpy");
static_assert(SC_AllowMemcpyRelocation<SFaultyTestEnum>::mValue, "[enum] may NOT be relocated by memcpy");
static_assert(SC_AllowMemcpyRelocation<SFaultyTestData>::mValue, "[data-structs] may NOT be relocated by memcpy");
static_assert(SC_AllowMemcpyRelocation<SFaultyTestStruct>::mValue, "[struct] may NOT be relocated by memcpy");
static_assert(SC_AllowMemcpyRelocation<SFaultyTestClass>::mValue, "[class] may NOT be relocated by memcpy");
static_assert(SC_AllowMemcpyRelocation<SFaultyTestUnion>::mValue, "[union] may NOT be relocated by memcpy");
static_assert(!SC_AllowMemcpyRelocation<SFaultyTestNotAllowed>::mValue, "[struct with nontrivial destructor] SHOULD be relocated by memcpy");

void SC_RelocateBytes(void* aDst, void* aSrc, size_t aCount)
{
	if (aDst == aSrc || aCount == 0)
		return;

	const ptrdiff_t signedDiff = uintptr_t(aDst) - uintptr_t(aSrc);
	const ptrdiff_t absDiff = (signedDiff >= 0) ? signedDiff : -signedDiff;

	if ((size_t)absDiff >= aCount)
	{
		SC_Memcpy(aDst, aSrc, aCount);
	}
	else
	{
		SC_Memmove(aDst, aSrc, aCount);
	}
}