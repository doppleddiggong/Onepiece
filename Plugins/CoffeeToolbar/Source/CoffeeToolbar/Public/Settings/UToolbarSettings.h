// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "FToolbarButtonInfo.h"
#include "Engine/DeveloperSettings.h"
#include "FToolbarButtonInfo.h"
#include "Folder/FFolderPathInfo.h"
#include "NetworkTest/FApiSendInfo.h"
#include "UToolbarSettings.generated.h"

/**
 * @file UToolbarSettings.h
 * @brief Coffee Toolbar 플러그인을 구성하기 위한 개발자 설정을 선언합니다.
 */

/**
 * @brief 툴바 버튼 배치와 기능 토글을 정의하는 개발자 설정입니다.
 */
UCLASS(Config=ToolbarSettings, defaultconfig)
class COFFEETOOLBAR_API UToolbarSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
	/** @brief 합리적인 기본값으로 설정 객체를 구성합니다. */
	UToolbarSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** @brief 프로젝트 설정 패널에서 사용되는 설정 카테고리를 반환합니다. */
    virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
    // Force project container so changes persist to DefaultToolbarSettings.ini
    virtual FName GetContainerName() const override { return TEXT("Project"); }

	/** @brief 설정 그룹에 대한 지역화된 섹션 이름을 반환합니다. */
	virtual FName GetSectionName()  const override { return TEXT("Level Selector"); }

	/**
	 * @brief 레벨 선택 헬퍼가 사용할 검색 루트 경로를 수집합니다.
	 * @param bFallbackToGame 사용자 지정 경로가 없을 때 게임 콘텐츠 디렉터리를 포함할지 여부입니다.
	 */
	static TArray<FName> GetSearchRoots(const bool bFallbackToGame = true);


	/** @brief true일 때 레벨 관련 툴바 기능을 활성화합니다. */
	UPROPERTY(EditAnywhere, Config, Category="Search")
	bool bEnableLevelFeature = true;
	/** @brief 레벨을 탐색할 때 추가로 확인할 디렉터리 목록입니다. */
	UPROPERTY(EditAnywhere, Config, Category="Search")
	TArray<FString> ExtraSearchPathsStrings;
	UPROPERTY(Transient)
	TArray<FDirectoryPath> ExtraSearchPaths;

	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	/** @brief true일 때 스크린샷 헬퍼 기능을 활성화합니다. */
	UPROPERTY(EditAnywhere, Config, Category="Screenshot")
	bool bEnableScreenshotFeature = true;



	
	/** @brief true일 때 명령 버튼 기능을 활성화합니다. */
	UPROPERTY(EditAnywhere, Config, Category="Toolbar", meta=(EditCondition="bEnableCommandFeature"))
	bool bEnableCommandFeature = true;
	/** @brief Slate 항목을 정의하는 툴바 버튼 설명자입니다. */
	UPROPERTY(EditAnywhere, Config, Category="Toolbar")
	TArray<FToolbarButtonInfo> ToolbarButtons;



	
	/** @brief true일 때 폴더 열기 기능을 활성화합니다. */
	UPROPERTY(EditAnywhere, Config, Category="Folder")
	bool bEnableFolderFeature = true;

	/** @brief 폴더 열기 기능에 사용될 경로 목록입니다. */
	UPROPERTY(EditAnywhere, Config, Category="Folder", meta=(EditCondition="bEnableFolderFeature"))
	TArray<FFolderPathInfo> FoldersToOpen;




	/** @brief true일 때 네트워크 테스트 기능을 활성화합니다. */
	UPROPERTY(EditAnywhere, Config, Category="Network Test")
	bool bEnableNetworkTestFeature = true;

	UPROPERTY(EditAnywhere, Config, Category="Network Test", meta=(EditCondition="bEnableNetworkTestFeature"))
	FString NetworkTestUrl = TEXT("http://127.0.0.1:8000");

	/** @brief 네트워크 테스트에 사용될 API 요청 목록입니다. */
	UPROPERTY(EditAnywhere, Config, Category="Network Test", meta=(EditCondition="bEnableNetworkTestFeature"))
	TArray<FApiSendInfo> NetworkTests;

	/** @brief 마지막으로 받은 OAuth2 access_token (자동으로 Authorization 헤더에 사용됨) */
	UPROPERTY(Config)
	FString LastAccessToken;
};


