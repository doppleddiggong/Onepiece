// Fill out your copyright notice in the Description page of Project Settings.


#include "ULingoGameInstance.h"

#include <string>

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "GameLogging.h"

ULingoGameInstance::ULingoGameInstance()
{

}

void ULingoGameInstance::Init()
{
	Super::Init();

	// 현재 사용하는 서브시스템을 가져오자
	IOnlineSubsystem* subsys = Online::GetSubsystem(GetWorld());
	if (subsys)
	{
		// 서브시스템의 인터페이스를 가져오자.
		sessionInterface = subsys->GetSessionInterface();
		// 세션 생성 성공시 호출되는 함수 등록
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
			this, &ULingoGameInstance::OnCreateSessionComplete);
		// 세션 조회 성공시 호출되는 함수 등록
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
			this, &ULingoGameInstance::OnFindSessionComplete);
		// 세션 참여 성공시 호출되는 함수 등록
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
			this, &ULingoGameInstance::OnJoinSessionComplete);
	}
}
//
// void ULingoGameInstance::SetPlayerRole(APlayerController* PlayerController, EPlayerRole Role)
// {
// 	if (!PlayerController)
// 	{
// 		PRINTLOG(TEXT("[FireGameInstance] SetPlayerRole: Invalid PlayerController"));
// 		return;
// 	}
//
// 	// PlayerState의 PlayerName을 Key로 사용 (맵 전환 시에도 유지됨)
// 	APlayerState* PS = PlayerController->GetPlayerState<APlayerState>();
// 	if (!PS)
// 	{
// 		PRINTLOG( TEXT("[FireGameInstance] SetPlayerRole: PlayerState not found!"));
// 		return;
// 	}
//
// 	FString PlayerKey = PS->GetPlayerName();
//
// 	// PlayerName이 비어있으면 PlayerId 사용 (Fallback)
// 	if (PlayerKey.IsEmpty())
// 	{
// 		PlayerKey = FString::Printf(TEXT("Player_%d"), PS->GetPlayerId());
// 		PRINTLOG( TEXT("[FireGameInstance] PlayerName empty, using PlayerId: %s"), *PlayerKey);
// 	}
//
// 	// PlayerRoleMap.Add(PlayerKey, Role);
//
// 	PRINTLOG( TEXT("[FireGameInstance] Player %s (Key: %s) selected role: %s"),
// 		*PlayerController->GetName(),
// 		*PlayerKey,
// 		*UEnum::GetValueAsString(Role));
// }
//
// EPlayerRole ULingoGameInstance::GetPlayerRole(APlayerController* PlayerController) const
// {
// 	if (!PlayerController)
// 	{
// 		PRINTLOG( TEXT("[FireGameInstance] GetPlayerRole: Invalid PlayerController"));
// 		return EPlayerRole::None;
// 	}
//
// 	// PlayerState의 PlayerName을 Key로 사용 (맵 전환 시에도 유지됨)
// 	APlayerState* PS = PlayerController->GetPlayerState<APlayerState>();
// 	if (!PS)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("[FireGameInstance] GetPlayerRole: PlayerState not found!"));
// 		return EPlayerRole::None;
// 	}
//
// 	FString PlayerKey = PS->GetPlayerName();
//
// 	// PlayerName이 비어있으면 PlayerId 사용 (Fallback)
// 	if (PlayerKey.IsEmpty())
// 	{
// 		PlayerKey = FString::Printf(TEXT("Player_%d"), PS->GetPlayerId());
// 		PRINTLOG( TEXT("[FireGameInstance] PlayerName empty, using PlayerId: %s"), *PlayerKey);
// 	}
//
// 	// TMap에서 Role 조회
// 	const EPlayerRole* FoundRole = PlayerRoleMap.Find(PlayerKey);
//
// 	if (FoundRole)
// 	{
// 		PRINTLOG( TEXT("[FireGameInstance] Found role for %s (Key: %s): %s"),
// 			*PlayerController->GetName(),
// 			*PlayerKey,
// 			*UEnum::GetValueAsString(*FoundRole));
// 		return *FoundRole;
// 	}
// 	else
// 	{
// 		PRINTLOG( TEXT("[FireGameInstance] No role found for %s (Key: %s), returning None"),
// 			*PlayerController->GetName(),
// 			*PlayerKey);
// 		return EPlayerRole::None;
// 	}
// }
//
// void ULingoGameInstance::DebugPrintPlayerRoles() const
// {
// 	PRINTLOG( TEXT("=== [FireGameInstance] Current Player Roles ==="));
//
// 	if (PlayerRoleMap.Num() == 0)
// 	{
// 		PRINTLOG( TEXT("  (No players registered)"));
// 		return;
// 	}
//
// 	for (const auto& Entry : PlayerRoleMap)
// 	{
// 		PRINTLOG( TEXT("  Key: %s -> Role: %s"),
// 			*Entry.Key,
// 			*UEnum::GetValueAsString(Entry.Value));
// 	}
//
// 	PRINTLOG( TEXT("==========================================="));
// }

void ULingoGameInstance::CreateMySession(FString displayName)
{
	// 세션을 만들기 위한 옵션 담을 변수
	FOnlineSessionSettings sessionSettings;
	// 현재 사용중인 서브시스템 이름 가져오자.
	FName subsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();
	// 만약에 서브시스템이 이름이 NULL 이면 Lan 을 이용하게 설정
	sessionSettings.bIsLANMatch = subsysName.IsEqual(FName(TEXT("NULL")));
	UE_LOG(LogTemp, Warning, TEXT("서브시스템 : %s"), *subsysName.ToString());

	// Steam 에선 필수 (bUseLobbiesIfAvailable, bUsesPresence)
	// Lobby 사용 여부 설정
	sessionSettings.bUseLobbiesIfAvailable = true;
	// 친구 상태를 확인할 수 있는 여부
	sessionSettings.bUsesPresence = true;

	// 세션 검색 허용 여부
	sessionSettings.bShouldAdvertise = true;
	// 세션 최대 참여 인원 설정 (임시로 2명)
	sessionSettings.NumPublicConnections = 2;
	// 커스텀 정보
	// displayName 을 Base64 로 변환
	displayName = StringBase64Encode(displayName);
	sessionSettings.Set(FName("DP_NAME"), displayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//sessionSettings.Set(FName("MAP_IDX"), 1, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// sessionSettings 이용해서 세션 생성
	FUniqueNetIdPtr netId =
		GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	sessionInterface->CreateSession(*netId, FName(displayName), sessionSettings);	
	//sessionInterface->CreateSession(0, FName(displayName), sessionSettings);

}

void ULingoGameInstance::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 생성 성공"), *sessionName.ToString());
		// 맵 으로 이동
		GetWorld()->ServerTravel(TEXT("/Game/StarterBundle/CollectionMaps/Map1?listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 생성 실패"), *sessionName.ToString());
	}
}

void ULingoGameInstance::FindOtherSession()
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 시작"));
	// sessionSearch 만들자.
	sessionSearch = MakeShared<FOnlineSessionSearch>();
	// 현재 사용중인 서브시스템 이름 가져오자.
	FName subsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();
	// 만약에 서브시스템이 이름이 NULL 이면 Lan 을 이용하게 설정
	sessionSearch->bIsLanQuery = subsysName.IsEqual(FName(TEXT("NULL")));
	// 어떤 옵션을 기준으로 검색
	sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	//sessionSearch->QuerySettings.Set(FName("DP_NAME"), FName("Wanted"), EOnlineComparisonOp::Equals);
	// 검색 갯수
	sessionSearch->MaxSearchResults = 100;
	// 위 설정을 가지고 세션 검색
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void ULingoGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 끝"));
	if (bWasSuccessful)
	{
		// 검색된 세션 결과들
		auto results = sessionSearch->SearchResults;
		for (int32 i = 0; i < results.Num(); i++)
		{
			// 방 제목 이름 담을 변수
			FString displayName;
			results[i].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), displayName);
			// displayName 을 UTF8 string 으로 변환
			displayName = StringBase64Decode(displayName);
			UE_LOG(LogTemp, Warning, TEXT("세션 - %i, 이름 : %s"), i, *displayName);
			// onFindComplete 에 들어있는 함수 실행
			onFindComplete.ExecuteIfBound(i, displayName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 조회 실패"));
	}

	onFindComplete.ExecuteIfBound(-1, FString());
}

void ULingoGameInstance::JoinOtherSession(int32 sessionIdx)
{
	// 검색된 세션 결과들
	auto results = sessionSearch->SearchResults;
	// 5.5 이후 부터 바꼈다...
	results[sessionIdx].Session.SessionSettings.bUseLobbiesIfAvailable = true;
	results[sessionIdx].Session.SessionSettings.bUsesPresence = true;

	// 세션 이름 가져오자.
	FString displayName;
	results[sessionIdx].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), displayName);

	// 세션 참여
	sessionInterface->JoinSession(0, FName(displayName), results[sessionIdx]);
}

void ULingoGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	// 만약 참여 성공 했다면
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		// 서버가 만들어 놓은 세션 URL 을 얻어오자.k
		FString url;
		sessionInterface->GetResolvedConnectString(sessionName, url);
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *url);
		// 서버가 있는 맵으로 이동 (최초 1번)
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel(url, TRAVEL_Absolute);
	}
}

FString ULingoGameInstance::StringBase64Encode(FString str)
{
	// str 을 std::string 로 변환
	std::string utf8String = TCHAR_TO_UTF8(*str);
	// utf8String 을 uint8 의 Array 변환
	TArray<uint8> arrayData = TArray<uint8>((uint8*)utf8String.c_str(), utf8String.length());

	return FBase64::Encode(arrayData);
}

FString ULingoGameInstance::StringBase64Decode(FString str)
{
	TArray<uint8> arrayData;
	FBase64::Decode(str, arrayData);
	std::string utf8String((char*)arrayData.GetData(), arrayData.Num());
	return UTF8_TO_TCHAR(utf8String.c_str());
}

void ULingoGameInstance::SetSelectCharacter(FString userName, int32 characterIdx)
{
	selectCharacter.Add(userName, characterIdx);
}

int32 ULingoGameInstance::GetSelectCharacter(FString userName)
{
	int32* value = selectCharacter.Find(userName);
	if (value == nullptr) return -1;

	return *value;
}
