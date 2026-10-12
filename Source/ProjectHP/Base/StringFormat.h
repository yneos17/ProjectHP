#pragma once

#include "CoreMinimal.h"

template <typename T>
auto ToString( const T& Value ) -> decltype( Value.ToString() ) { return Value.ToString(); }

template <typename T>
FORCEINLINE typename TEnableIf<TIsArithmetic<T>::Value, FString>::Type
ToString( T Value )
{
	if constexpr( TIsFloatingPoint<T>::Value ) return FString::SanitizeFloat( Value );
	else return FString::Printf( TEXT( "%lld" ), (int64)Value );
}

template <typename T>
FORCEINLINE typename TEnableIf<TIsEnum<T>::Value, FString>::Type
ToString( T Value )
{
	const UEnum* EnumPtr = StaticEnum<T>();
	return EnumPtr ? EnumPtr->GetNameStringByValue( (int64)Value ) : FString::FromInt( (int32)Value );
}

FORCEINLINE FString ToString( const FName& Value ) { return Value.ToString(); }
FORCEINLINE FString ToString( const FString& Value ) { return Value; }
FORCEINLINE FString ToString( const FText& Value ) { return Value.ToString(); }
FORCEINLINE FString ToString( bool Value ) { return Value ? TEXT( "True" ) : TEXT( "False" ); }

template<typename... Args>
FString FormatString( FString Format, Args... args )
{
	TArray<FString> Params = { ToString( args )... };
	for( const FString& Param : Params )
	{
		int32 Index = Format.Find( TEXT( "%#" ) );
		if( Index == INDEX_NONE ) break;

		Format.RemoveAt( Index, 2 );
		Format.InsertAt( Index, Param );
	}
	return Format;
}