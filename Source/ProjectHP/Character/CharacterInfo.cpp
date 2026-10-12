#include "Character/CharacterInfo.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

static const FString CharacterInfoFolderPath = TEXT( "StaticInfo/CharacterInfo" );

FCharacterInfo::FCharacterInfo()
{
	Key = 0;
}

UCharacterInfoManager* UCharacterInfoManager::Get()
{
	if( GEngine == nullptr )
		return nullptr;

	return GEngine->GetEngineSubsystem<UCharacterInfoManager>();
}

void UCharacterInfoManager::Initialize( FSubsystemCollectionBase& Collection )
{
	Super::Initialize( Collection );
	LoadCharacterInfoFromXML();
}

const FCharacterInfo* UCharacterInfoManager::GetCharacterInfo( const int32 Key ) const
{
    return CharacterInfoMapByIndex.Find( Key );
}

const FCharacterInfo* UCharacterInfoManager::GetCharacterInfo( const FName& CharacterKey ) const
{
    return CharacterInfoMapByStringKey.Find( CharacterKey );
}

bool UCharacterInfoManager::LoadCharacterInfoFromXML()
{
    const FString DataFolderPath = FPaths::ProjectContentDir() + CharacterInfoFolderPath;

    TArray<FString> XmlFileList;
    IFileManager::Get().FindFiles( XmlFileList, *DataFolderPath, TEXT( "xml" ) );

    TMap<int32, FCharacterInfo> TempCharacterInfoMapByIndex;
    TMap<FName, FCharacterInfo> TempCharacterInfoMapByStringKey;

    for( const FString& FileName : XmlFileList )
    {
        FString FullPath = FormatString( "%#/%#", DataFolderPath, FileName );
        FXmlFile* XmlFile = new FXmlFile( FullPath );

        if( XmlFile->IsValid() == false )
        {
			MJ_ASSERT_DEV( "김민진", false, "CharacterInfo XML 파일 중 정상적으로 Load 되지 않는 파일이 있습니다. [%#]", FullPath );
            return false;
        }

		const TArray<FXmlNode*>& CharacterInfoNodeList = XmlFile->GetRootNode()->GetChildrenNodes();
		for( const FXmlNode* CharacterInfoNode : CharacterInfoNodeList )
		{
			FCharacterInfo CharacterInfo;

			CharacterInfo.StringKey = FName( CharacterInfoNode->GetTag() );

			const TArray<FXmlAttribute>& CharacterInfoAttributeList = CharacterInfoNode->GetAttributes();
			for( const FXmlAttribute& CharcterInfoAttribute : CharacterInfoAttributeList )
			{
				const FString& AttributeTag = CharcterInfoAttribute.GetTag();
				if( AttributeTag.Equals( TEXT( "Key" ), ESearchCase::CaseSensitive ) )
				{
					XmlUtil::Parse( CharcterInfoAttribute, CharacterInfo.Key );
				}
				else if( AttributeTag.Equals( TEXT( "CharacterClass" ), ESearchCase::CaseSensitive ) )
				{
					const FString& ClassName = CharcterInfoAttribute.GetValue();
					if( ClassName.IsEmpty() )
					{
						MJ_ASSERT_DEV( "김민진", false, "CharacterInfo Load중 CharacterClass가 비어있습니다. 데이터를 확인해주세요." );
						return false;
					}

					FString ClassFullName = FormatString( "/Game/BluePrint/Character/%#.%#_C", ClassName, ClassName );
					UClass* CharacterClass = StaticLoadClass( ACharacter::StaticClass(), nullptr, *ClassFullName, nullptr, LOAD_None, nullptr );
					if( CharacterClass == nullptr )
					{
						MJ_ASSERT_DEV( "김민진", false, "CharacterInfo Load중 CharacterClass를 찾을 수 없습니다. 데이터를 확인해주세요." );
						return false;
					}

					CharacterInfo.CharacterClass = CharacterClass;
				}
				else
				{
					MJ_ASSERT_DEV( "김민진", false, "CharacterInfo Load중 Attribute[%#]를 찾을 수 없습니다. 데이터를 확인해주세요.", AttributeTag );
					return false;
				}
			}

			if( TempCharacterInfoMapByIndex.Find( CharacterInfo.Key ) != nullptr )
			{
				MJ_ASSERT_DEV( "김민진", false, "CharacterInfo Load중 중복되는 Key[%#]가 있어 Load에 실패했습니다.", CharacterInfo.Key );
				return false;
			}

			if( TempCharacterInfoMapByStringKey.Find( CharacterInfo.StringKey ) != nullptr )
			{
				MJ_ASSERT_DEV( "김민진", false, "CharacterInfo Load중 중복되는 StringKey[%#]가 있어 Load에 실패했습니다.", CharacterInfo.StringKey );
				return false;
			}

			TempCharacterInfoMapByIndex.Add( CharacterInfo.Key, CharacterInfo );
			TempCharacterInfoMapByStringKey.Add( CharacterInfo.StringKey, CharacterInfo );
		}

        delete XmlFile;
    }

    CharacterInfoMapByIndex = MoveTemp( TempCharacterInfoMapByIndex );
    CharacterInfoMapByStringKey = MoveTemp( TempCharacterInfoMapByStringKey );
	
	return true;
}