// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace GameLayer
{
	static const int32 Loading = 400;
	static const int32 LoadingCircle = 500;
	static const int32 ToastManager = 600;
}

/// @brief 게임 전역에서 공유하는 GameplayTag 상수를 정의합니다.
namespace GameTags
{
	/// @brief 플레이어 액터를 식별하기 위한 태그입니다.
	static const FName Player = TEXT("Player");

	/// @brief 문 및 문짝과 관련된 액터에 부여되는 태그입니다.
	static const FName Door = TEXT("Door");

	/// @brief 건물 액터를 구분할 때 사용하는 태그입니다.
	static const FName Building = TEXT("Building");
}

namespace GameMessage
{
	static const FString GameStart = TEXT("GameStart");
	static const FString GameEnd	= TEXT("GameEnd");
	static const FString IntroStart = TEXT("IntroStart");
	static const FString OutroStart = TEXT("OutroStart");    

	static const FString ScenarioStage1Start = TEXT("ScenarioStage1Start");    
	static const FString ScenarioStage1End = TEXT("ScenarioStage1End");    

	static const FString ScenarioStage2Start = TEXT("ScenarioStage2Start");    
	static const FString ScenarioStage2End = TEXT("ScenarioStage2End");    

	static const FString ScenarioStage3Start = TEXT("ScenarioStage3Start");    
	static const FString ScenarioStage3End = TEXT("ScenarioStage3End");    

	static const FString ScenarioStage4Start = TEXT("ScenarioStage4Start");    
	static const FString ScenarioStage4End = TEXT("ScenarioStage4End");    


}