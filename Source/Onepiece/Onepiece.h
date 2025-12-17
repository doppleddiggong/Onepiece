// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace GameLayer
{
	static const int32 Popup = 300;
	static const int32 Toast = 400;
	static const int32 Loading = 400;
	static const int32 LoadingCircle = 500;
	static const int32 ToastManager = 600;
}

namespace DoorGroup
{
	// door_0 : 입구
	static const int32 Intro = 0;

	// door_1 : Step1(Read) 중간 게이트
	static const int32 Step1_Mid = 1;
	// door_2 : Step1(Read) 엔드 게이트
	static const int32 Step1_End = 2;

	// door_100 : Step2(Listen) 엔드 게이트
	static const int32 Step2_End = 100;

	// door_200 : Step3(Speak) 엔드 게이트
	static const int32 Step3_End = 200;

	// door_300 : Step4(Write) 엔드 게이트
	static const int32 Step4_End = 300;
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

	static const FString ReadStageStart		= TEXT("Read Stage Start");    
	static const FString ReadStageEnd		= TEXT("Read Stage Clear");    

	static const FString ListenStageStart	= TEXT("Listen Stage Start");    
	static const FString ListenStageEnd		= TEXT("Listen Stage Clear");    

	static const FString WriteStageStart	= TEXT("Write Stage Start");    
	static const FString WriteStageEnd		= TEXT("Write Stage Clear");    

	static const FString SpeakStageStart	= TEXT("Speak Stage Start");
	static const FString SpeakStageEnd		= TEXT("Speak Stage Clear");


	static const FString Holder_Success		= TEXT("Perfect! You placed the block right. Let's keep going!");
	static const FString Holder_Fail		= TEXT("Oops! That block doesn't go there.");    
}