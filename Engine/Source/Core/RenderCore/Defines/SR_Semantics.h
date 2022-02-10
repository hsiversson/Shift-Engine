#pragma once

namespace SR_Semantic
{
	enum Semantic
	{
#define SR_SEMANTIC(aName, aId, ...) aName = aId,
#include "SR_SemanticsList.h"
		SEMANTIC_COUNT
	};


}