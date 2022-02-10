#pragma once

#ifndef SR_SEMANTIC
#	define SR_SEMANTIC(...)
#endif
//			Name					,Id,	IsArray,	Format
SR_SEMANTIC(SR_Dummy,				0,		0,			RGBA32_FLOAT	)
SR_SEMANTIC(SR_Position,			1,		0,			RGBA32_FLOAT	)
SR_SEMANTIC(SR_Depth,				2,		0,			R32_FLOAT		)
SR_SEMANTIC(SR_PixelCoord,			3,		0,			RGBA32_FLOAT	)
SR_SEMANTIC(SR_Target,				4,		1,			RGBA32_FLOAT	)
SR_SEMANTIC(SR_FrontFace,			5,		0,			R1_BOOL			)
SR_SEMANTIC(SR_VertexId,			6,		0,			R32_UINT		)
SR_SEMANTIC(SR_DispatchThreadId,	7,		0,			RGB32_UINT		)
SR_SEMANTIC(SR_GroupThreadId,		8,		0,			RGB32_UINT		)
SR_SEMANTIC(SR_GroupId,				9,		0,			RGB32_UINT		)
SR_SEMANTIC(SR_GroupIndex,			10,		0,			R1_BOOL			)

#undef SR_SEMANTIC