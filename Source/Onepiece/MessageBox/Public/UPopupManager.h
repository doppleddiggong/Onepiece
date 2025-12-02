// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EPopupType.h"
#include "Macro.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UPopupManager.generated.h"

/**
 * @brief 팝업 관리자
 *
 * 게임 내 모든 팝업을 관리하는 LocalPlayerSubsystem입니다.
 * - 팝업 스택 관리 (팝업 위에 팝업 표시 가능)
 * - 타입별 팝업 인스턴스 관리
 * - 팝업 생성, 표시, 숨김, 쿼리 기능 제공
 *
 * 사용 예시:
 * @code
 * UPopupManager* PopupMgr = UPopupManager::Get(GetWorld());
 * PopupMgr->ShowPopup(EPopupType::MsgBox);
 * PopupMgr->HideCurrentPopup();
 * @endcode
 */



UCLASS()
class ONEPIECE_API UPopupManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_LOCALPLAYER_SUBSYSTEM_GETTER_INLINE(UPopupManager);

	UPopupManager();

public:
	// ========================================
	// 팝업 클래스 설정
	// ========================================

	/// 팝업 타입별 위젯 클래스 맵 (에디터에서 설정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Popup|Setup")
	TMap<EPopupType, TSubclassOf<UUserWidget>> PopupClassMap;

private:
	// ========================================
	// 팝업 인스턴스 관리
	// ========================================

	/// 팝업 타입별 위젯 인스턴스 맵
	UPROPERTY()
	TMap<EPopupType, UUserWidget*> PopupWidgetMap;

	/// 팝업 스택 (표시 순서, 마지막이 최상단)
	UPROPERTY()
	TArray<EPopupType> PopupStack;

public:
	// ========================================
	// 범용 팝업 관리 함수
	// ========================================

	/**
	 * @brief 팝업 표시
	 * @param Type 표시할 팝업 타입
	 * @return 생성/표시된 팝업 위젯 (실패 시 nullptr)
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	UUserWidget* ShowPopup(EPopupType Type);

	/**
	 * @brief 특정 팝업 숨기기
	 * @param Type 숨길 팝업 타입
	 * @param bDestroyWidget 위젯을 완전히 제거할지 여부 (false면 뷰포트에서만 제거)
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	void HidePopup(EPopupType Type, bool bDestroyWidget = false);

	/**
	 * @brief 현재 활성화된 팝업 숨기기 (스택 최상단)
	 * @param bDestroyWidget 위젯을 완전히 제거할지 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	void HideCurrentPopup(bool bDestroyWidget = false);

	/**
	 * @brief 모든 팝업 숨기기
	 * @param bDestroyWidgets 위젯들을 완전히 제거할지 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	void HideAllPopups(bool bDestroyWidgets = false);

	// ========================================
	// 팝업 쿼리 함수
	// ========================================

	/**
	 * @brief 현재 활성화된 팝업 타입 가져오기 (스택 최상단)
	 * @param OutType 현재 팝업 타입 (출력)
	 * @return 활성화된 팝업이 있으면 true
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	bool GetCurrentPopupType(EPopupType& OutType) const;

	/**
	 * @brief 현재 활성화된 팝업 위젯 가져오기 (스택 최상단)
	 * @return 현재 팝업 위젯 (없으면 nullptr)
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	UUserWidget* GetCurrentPopupWidget() const;

	/**
	 * @brief 특정 타입의 팝업 위젯 가져오기
	 * @param Type 팝업 타입
	 * @return 팝업 위젯 (없으면 nullptr)
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	UUserWidget* GetPopupWidget(EPopupType Type) const;

	/**
	 * @brief 특정 타입의 팝업이 현재 스택에 있는지 확인
	 * @param Type 팝업 타입
	 * @return 스택에 있으면 true
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	bool IsPopupInStack(EPopupType Type) const;

	/**
	 * @brief 활성화된 팝업 개수 가져오기
	 * @return 팝업 스택 크기
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	int32 GetPopupStackCount() const;

	// ========================================
	// 메시지 박스 전용 함수 (편의 함수)
	// ========================================

	/**
	 * @brief 메시지 박스 표시 (OK 버튼만)
	 * @param InTitle 제목
	 * @param InDescription 설명
	 * @param InOkDelegate OK 버튼 클릭 델리게이트
	 * @note C++에서만 사용 가능 (델리게이트는 블루프린트에 노출 불가)
	 */
	void ShowMsgBox(
		const FString& InTitle,
		const FString& InDescription,
		EMsgBoxType InType,
		const FOnMsgBoxOkDelegate& InOkDelegate);

	/**
	 * @brief 메시지 박스 표시 (OK, Cancel 버튼)
	 * @param InTitle 제목
	 * @param InDescription 설명
	 * @param InOkDelegate OK 버튼 클릭 델리게이트
	 * @param InCancelDelegate Cancel 버튼 클릭 델리게이트
	 * @note C++에서만 사용 가능 (델리게이트는 블루프린트에 노출 불가)
	 */
	void ShowMsgBox(
		const FString& InTitle,
		const FString& InDescription,
		EMsgBoxType InType,
		const FOnMsgBoxOkDelegate& InOkDelegate,
		const FOnMsgBoxCancelDelegate& InCancelDelegate);

	/**
	 * @brief 메시지 박스 표시 (블루프린트용 - 델리게이트 없음)
	 * @param InTitle 제목
	 * @param InDescription 설명
	 * @param InType 메시지 박스 타입
	 * @note 블루프린트에서 사용. 버튼 이벤트는 위젯 블루프린트에서 직접 처리
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup|MessageBox")
	void ShowMsgBoxSimple(
		const FString& InTitle,
		const FString& InDescription,
		EMsgBoxType InType);

	// ========================================
	// Result 팝업 전용 함수
	// ========================================

	/**
	 * @brief Result 팝업 표시
	 * @note Result 팝업은 InitPopup을 호출하여 초기화합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup|Result")
	void ShowResult();

protected:
	// ========================================
	// 내부 헬퍼 함수
	// ========================================

	/**
	 * @brief 팝업 위젯 생성 또는 가져오기
	 * @param Type 팝업 타입
	 * @return 팝업 위젯 (실패 시 nullptr)
	 */
	UUserWidget* EnsurePopupWidget(EPopupType Type);

	/**
	 * @brief 팝업을 스택에 추가
	 * @param Type 팝업 타입
	 */
	void PushPopupToStack(EPopupType Type);

	/**
	 * @brief 팝업을 스택에서 제거
	 * @param Type 팝업 타입
	 */
	void RemovePopupFromStack(EPopupType Type);
};