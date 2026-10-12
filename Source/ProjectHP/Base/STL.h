#pragma once

#include "CoreMinimal.h"

template <typename T, uint32 Size>
using TScopeArray = TArray<T, TInlineAllocator<Size>>;