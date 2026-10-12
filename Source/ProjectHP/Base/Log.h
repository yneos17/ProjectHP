#pragma once

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING
#define MJ_ASSERT_DEV(developer, condition, message, ...) \
    do \
    { \
        if( ( condition ) == false ) \
        { \
            FString FormattedMsg = FormatString(TEXT(message), ##__VA_ARGS__); \
            ensureMsgf(false, TEXT("[%s] %s"), TEXT(developer), *FormattedMsg); \
        } \
    } while( false )
#else
#define MJ_ASSERT_DEV(developer, condition, message, ...)
#endif

#if !UE_BUILD_SHIPPING
#define MJ_LOG_DEV(developer, message, ...) \
    do \
    { \
        FString FormattedMsg = FormatString(TEXT(message), ##__VA_ARGS__); \
        UE_LOG(LogTemp, Log, TEXT("[%s] %s"), TEXT(developer), *FormatString(TEXT(message), ##__VA_ARGS__)); \
    } while( false )
#else
#define MJ_LOG_DEV(developer, message, ...)
#endif

#define MJ_LOG_SERVICE(developer, message, ...) \
    do \
    { \
        FString FormattedMsg = FormatString(TEXT(message), ##__VA_ARGS__); \
        UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), TEXT(developer), *FormattedMsg); \
    } while( false )