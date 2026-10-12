#include "MathUtil.h"

namespace MathUtil
{
	float LerpDegree( const float A, const float B, const float Alpha )
	{
		const float DeltaAngle = FMath::FindDeltaAngleDegrees( A, B );
		float Result = A + ( DeltaAngle * Alpha );
		return FMath::UnwindDegrees( Result );
	}

	float LerpRadian( const float A, const float B, const float Alpha )
	{
		const float DeltaAngle = FMath::FindDeltaAngleRadians( A, B );
		float Result = A + ( DeltaAngle * Alpha );
		return FMath::UnwindRadians( Result );
	}
}