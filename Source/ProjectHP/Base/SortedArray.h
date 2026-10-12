#pragma once

#include "CoreMinimal.h"
#include "Pch.h"

template<typename Type>
class TSortedArray
{
public:
	FORCEINLINE void Push( const Type& NewDataElement );
	FORCEINLINE void Push( Type&& NewDataElement ) { Push( NewDataElement ); }
	FORCEINLINE void Add( const Type& NewDataElement ) { Push( NewDataElement ); }
	FORCEINLINE void Add( Type&& NewDataElement ) { Push( NewDataElement ); }

	FORCEINLINE int32 Num() const;

	FORCEINLINE Type& operator[]( const int32 Index ) UE_LIFETIMEBOUND;
	FORCEINLINE const Type& operator[]( const int32 Index ) const UE_LIFETIMEBOUND;

	FORCEINLINE bool operator==( const TSortedArray<Type>& Other ) const;

private:
	TArray<Type> Data;
};

template<typename Type>
void TSortedArray<Type>::Push( const Type& NewDataElement )
{
	int32 InsertIndex = Algo::LowerBound( Data, NewDataElement );
	Data.Insert( NewDataElement, InsertIndex );
}

template<typename Type>
int32 TSortedArray<Type>::Num() const
{
	return Data.Num();
}

template<typename Type>
Type& TSortedArray<Type>::operator[]( const int32 Index )
{
	return Data[Index];
}

template<typename Type>
const Type& TSortedArray<Type>::operator[]( const int32 Index ) const
{
	return Data[Index];
}

template<typename Type>
bool TSortedArray<Type>::operator==( const TSortedArray<Type>& Other ) const
{
	const int32 DataNum = Data.Num();
	if( DataNum != Other.Data.Num() )
		return false;

	for( int32 DataIndex = 0; DataIndex < DataNum; ++DataIndex )
	{
		if( Data[DataIndex] != Other.Data[DataIndex] )
			return false;
	}

	return true;
}