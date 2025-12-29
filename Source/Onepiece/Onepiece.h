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
	static const int32 Step1_Tutorial = 0;

	// door_1 : Step1(Read) 중간 게이트
	static const int32 Step1_Mid = 1;
	// door_2 : Step1(Read) 엔드 게이트
	static const int32 Step1_End = 2;

	// door_100 : Step2(Listen) 엔드 게이트
	static const int32 Step2_Tutorial = 99;
	static const int32 Step2_End = 100;

	// door_200 : Step3(Speak) 엔드 게이트
	static const int32 Step3_Tutorial = 199;
	static const int32 Step3_End = 200;

	static const int32 Step4_Tutorial = 200;
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

namespace DefineData
{
	static const int32 BotID = 99999;
	static const FString BotName = TEXT("[KLingo]");

	static const FString AI = TEXT("AI");

	static const FString Daily = TEXT("Daily");
	
	static const float LineHeightPercentage = 1.2f;

	static const FString DailySystemPrompt = TEXT(R"(                                                                                     
	  너는 게임 클라이언트에서 사용하는 단어 데이터 생성기다.                                                                               

	  반드시 다음 규칙을 지켜야 한다.                                                                                                       
	  
	  1. 출력은 하나의 일반 문자열이다.                                                                                                     
	  2. 설명, 해설, 인사말을 절대 포함하지 않는다.                                                                                         
	  3. JSON, 마크다운, 번호 목록을 사용하지 않는다.                                                                                       
	  4. 줄바꿈을 포함하지 않는다.                                                                                                          
	  5. 출력에는 한글, 영문, '-', '|' 문자만 포함할 수 있다.                                                                               
	  6. 규칙을 지킬 수 없으면 빈 문자열을 출력한다.                                                                                        

	  생성 규칙:                                                                                                                            
	  - 사용자의 요청에서 단어 개수(N)를 판단한다.                                                                                          
	  - 숫자가 명시되면 그 숫자를 사용한다.                                                                                                 
	  - 숫자가 없으면 기본값은 3이다.                                                                                                       
	  - 정확히 N개의 쉬운 한국어 단어를 생성한다.                                                                                           
	  - 각 단어는 초급 학습자가 이해할 수 있는 수준으로 작성한다.                                                                           

	  출력 형식:                                                                                                                            
	  - 각 단어는 "한글|영어|영문발음" 순서로 작성한다.                                                                                     
	  - 영어는 대문자로 작성한다.                                                                                                           
	  - 영문발음은 첫 글자만 대문자로 하고 음절은 하이픈(-)으로 구분한다.                                                                   
	  - 여러 단어는 '|' 문자로 이어서 출력한다.                                                                                             
	  - 마지막에 구분 문자를 추가하지 않는다.                                                                                               

	  예시:                                                                                                                                 
	  입력: "단어 2개 만들어줘"                                                                                                             
	  출력: "개|DOG|Gae|고양이|CAT|Go-yang-i"                                                                                               

	  입력: "과일 이름 3개"                                                                                                                 
	  출력: "사과|APPLE|Sa-gwa|바나나|BANANA|Ba-na-na|딸기|STRAWBERRY|Ttal-gi")");
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