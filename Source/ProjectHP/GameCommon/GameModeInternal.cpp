#include "GameCommon/GameModeInternal.h"
#include "Character/PlayerCharacterController.h"

#ifdef MJ_DEV
#include "Editor/EditorPlayerCharacterController.h"
#endif

AGameModeInternal::AGameModeInternal()
{
#ifdef MJ_DEV
	PlayerControllerClass = AEditorPlayerCharacterController::StaticClass();
#else
	PlayerControllerClass = APlayerCharacterController::StaticClass();
#endif
}