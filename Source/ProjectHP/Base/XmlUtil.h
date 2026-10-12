#pragma once

#include "CoreMinimal.h"
#include "XmlNode.h"

class FXmlAttribute;

namespace XmlUtil
{
    bool Parse( const FXmlAttribute& Attribute, FString& OutValue );
    bool Parse( const FXmlAttribute& Attribute, float& OutValue );
	bool Parse( const FXmlAttribute& Attribute, int32& OutValue );
    bool Parse( const FXmlAttribute& Attribute, FVector2D& OutValue );
    bool Parse( const FXmlAttribute& Attribute, FVector& OutValue );

    template<typename T>
    bool Parse( const FXmlAttribute& Attribute, T& OutValue );
}

namespace XmlUtil
{
    template<typename T>
    bool Parse( const FXmlAttribute& Attribute, T& OutValue )
    {
        static_assert( TIsEnum<T>::Value, "ParseEnum can only be used with enumeration types." );

        const UEnum* EnumPtr = StaticEnum<T>();
        if( !EnumPtr )
            return false;

        const FString EnumStringValue = Attribute.GetValue();
        const int64 EnumeratorValue = EnumPtr->GetValueByNameString( EnumStringValue );

        if( EnumeratorValue != INDEX_NONE )
        {
            OutValue = static_cast<T>( EnumeratorValue );
            return true;
        }

        return false;
    }
}