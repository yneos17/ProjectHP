#include "XmlUtil.h"

namespace XmlUtil
{
    bool Parse( const FXmlAttribute& Attribute, FString& OutValue )
    {
        OutValue = Attribute.GetValue();
        return true;
    }

    bool Parse( const FXmlAttribute& Attribute, float& OutValue )
    {
        OutValue = FCString::Atof( *Attribute.GetValue() );
        return true;
    }

    bool Parse( const FXmlAttribute& Attribute, int32& OutValue )
    {
        OutValue = FCString::Atoi( *Attribute.GetValue() );
        return true;
    }

    bool Parse( const FXmlAttribute& Attribute, FVector2D& OutValue )
    {
        FString RawString = Attribute.GetValue().TrimStartAndEnd();
        if( RawString.IsEmpty() ) return false;

        RawString.ReplaceInline( TEXT( "," ), TEXT( " " ) );

        TArray<FString> Tokens;
        RawString.ParseIntoArrayWS( Tokens );

        if( Tokens.Num() != 2 )
            return false;

		OutValue.X = FCString::Atof( *Tokens[0] );
		OutValue.Y = FCString::Atof( *Tokens[1] );
		return true;
    }

    bool Parse( const FXmlAttribute& Attribute, FVector& OutValue )
    {
        FString RawString = Attribute.GetValue().TrimStartAndEnd();
        if( RawString.IsEmpty() ) return false;

        RawString.ReplaceInline( TEXT( "," ), TEXT( " " ) );

        TArray<FString> Tokens;
        RawString.ParseIntoArrayWS( Tokens );

        if( Tokens.Num() != 2 )
            return false;

		OutValue.X = FCString::Atof( *Tokens[0] );
		OutValue.Y = FCString::Atof( *Tokens[1] );
		OutValue.Z = FCString::Atof( *Tokens[2] );
		return true;
    }
}