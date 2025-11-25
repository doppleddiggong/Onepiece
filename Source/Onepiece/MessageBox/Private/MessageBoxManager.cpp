// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Onepiece/MessageBox/Public/MessageBoxManager.h"

#include "FComponentHelper.h"
#include "GameLogging.h"
#include "UKLingoNetworkSystem.h"
#include "Onepiece/Onepiece.h"
#include "Onepiece/MessageBox/Public/MessageBox.h"

#define MESSAGEBOX_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_MessageBox.WBP_MessageBox_C")

UMessageBoxManager::UMessageBoxManager()
{
	MessageBoxClass = FComponentHelper::LoadClass<UMessageBox>(MESSAGEBOX_PATH);
}

void UMessageBoxManager::ShowMessageBox(EMessageBoxType Type, const FString& Title, const FString& Description)
{
	if (Title.IsEmpty() || Description.IsEmpty())
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	EnsureWidgetForWorld(World);
	// 텍스트 설정
	if (MessageBox)
	{
		MessageBox->SetTitle(Title);
		MessageBox->SetDesc(Description);

		// 타입 별로 다르게 처리
		switch (Type)
		{
		case EMessageBoxType::Info:
			{
				// 유저이름 칸 삭제
				MessageBox->SetNameFieldVisibility(false);
			}
			break;
			
		case EMessageBoxType::Register:
			{
				MessageBox->SetNameFieldVisibility(true);
				MessageBox->OnUserNameRegister.AddDynamic(this, &UMessageBoxManager::RegisterUserName);
			}
			break;
			
		case EMessageBoxType::LogIn:
			{
				MessageBox->SetNameFieldVisibility(true);
				MessageBox->OnUserNameRegister.AddDynamic(this, &UMessageBoxManager::GetUserToken);
			}
			break;
		}
	}
}

void UMessageBoxManager::HideMessageBox()
{
	UWorld* World = GetWorld();
	
	if (!World)
		return;

	if (!MessageBox)
		return;

	if (MessageBox->GetWorld() != World)
		return;

	MessageBox->OnClickCancel();
}

void UMessageBoxManager::EnsureWidgetForWorld(UWorld* World)
{
	if (World == nullptr || !World->IsGameWorld())
		return;
	
	// 위젯이 유효하고 같은 월드이며, 뷰포트에 추가되어 있는지 확인
	if (IsValid(MessageBox) &&
		MessageBox->GetWorld() == World &&
		MessageBox->IsInViewport())
	{
		return;
	}

	// 기존 위젯이 있으면 정리
	if (MessageBox)
	{
		if (MessageBox->IsInViewport())
			MessageBox->RemoveFromParent();
		
		MessageBox = nullptr;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer == nullptr)
		return;
	
	APlayerController* PC = LocalPlayer->GetPlayerController(World);
	if (PC == nullptr)
		return;
	
	if (!MessageBoxClass)
		return;
	
	UMessageBox* NewWidget = CreateWidget<UMessageBox>(PC, MessageBoxClass);
	if (!NewWidget)
		return;
	
	NewWidget->AddToViewport(GameLayer::ToastManager);
	
	MessageBox = NewWidget;
}

void UMessageBoxManager::RegisterUserName(const FString& UserInput)
{
	if (UserInput.IsEmpty()) return;

	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		PRINTLOG(TEXT("[TEST] RequestUserRegister - UserName: %s"), *UserInput);
		KLingoNetwork->RequestUserRegister(
			UserInput,
			FResponseUserRegisterDelegate::CreateUObject(this,
				&UMessageBoxManager::OnResponseUserRegister)
		);
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
}

void UMessageBoxManager::GetUserToken(const FString& UserInput)
{
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		PRINTLOG(TEXT("[TEST] RequestUserToken - UserName: %s"), *UserInput);
		KLingoNetwork->RequestUserToken(
			UserInput,
			FResponseUserTokenDelegate::CreateUObject(this, &UMessageBoxManager::OnResponseUserToken)
		);
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
	
}

void UMessageBoxManager::OnResponseUserRegister(FResponseUserRegister& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- User Register SUCCESS ---"));
		ResponseData.PrintData();
		
		// 가입 성공 시 토큰 발급
		GetUserToken(ResponseData.username);
	}
	else
	{
		PRINTLOG(TEXT("--- User Register FAILED ---"));
	}
}

void UMessageBoxManager::OnResponseUserToken(FResponseUserToken& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- User Token SUCCESS ---"));
		ResponseData.PrintData();
		PRINTLOG(TEXT("Token: %s"), *ResponseData.access_token);
		
		// 토큰 발급 성공 시 로비맵으로
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyMap"));
	}
	else
	{
		PRINTLOG(TEXT("--- User Token FAILED ---"));
	}
}
