#include "FMathSub.h"

namespace FMathSub
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

	float FindDeltaAngleDegreesWithThreshold( const float A, const float B, const float ThresholdAngle )
	{
		const float RangeMinimum = -180.0f + ThresholdAngle;

		float DifferenceAngle = FMath::Fmod( B - A - RangeMinimum, 360.0f );
		if( DifferenceAngle < 0.0f )
			DifferenceAngle += 360.0f;

		return DifferenceAngle + RangeMinimum;
	}
}