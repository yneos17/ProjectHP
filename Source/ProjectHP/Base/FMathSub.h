#pragma once

#include "CoreMinimal.h"

namespace FMathSub
{
	float LerpDegree( const float A, const float B, const float Alpha );
	float LerpRadian( const float A, const float B, const float Alpha );

	float FindDeltaAngleDegreesWithThreshold( const float A, const float B, const float ThresholdAngle );
}