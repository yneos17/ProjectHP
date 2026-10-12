#pragma once

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING
#define MJ_DEV
#endif

#if WITH_EDITOR
#define MJ_EDITOR
#endif