/**
 * @file EGameSoundType.h
 * @brief EGameSoundType 클래스를 선언합니다.
 */
#pragma once

UENUM(BlueprintType)
enum class EGameSoundType : uint8
{
	None,

	BGM_Start,
	BGM_Main,

	UI_PopupOpen,
	UI_QuestStart,
	UI_Success,
	UI_Failure,
	UI_Interation,

	Speak_the_Word_in_korean,

	Hook,
	OpenDoor,
};
