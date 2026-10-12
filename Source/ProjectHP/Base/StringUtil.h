#pragma once

#include "CoreMinimal.h"

namespace StringUtil
{
    template<typename T>
    bool ConvertStringToEnum( const FString& StringValue, T& OutValue );
}

namespace StringUtil
{
    template<typename T>
    bool ConvertStringToEnum( const FString& StringValue, T& OutValue )
    {
        const UEnum* EnumPtr = StaticEnum<T>();
        if( EnumPtr == nullptr )
            return false;

        int64 EnumIndex = EnumPtr->GetValueByNameString( StringValue );

        if( EnumIndex != INDEX_NONE )
        {
            OutValue = static_cast<T>( EnumIndex );
            return true;
        }

        return false;
    }
}