/**
 * @file EGameSoundType.h
 * @brief EGameSoundType 클래스를 선언합니다.
 */
#pragma once

UENUM(BlueprintType)
enum class EGameSoundType : uint8
{
	Click,
	Diaglog,
	Disolve,

	Cmd_Approach,
	Enter_Game,
	Door_Open,

	What_is_your_name,
	Where_are_you_from,
	What_is_the_purpose_of_your_visit,
	How_long_will_you_stay,
	Where_will_you_be_staying
};
