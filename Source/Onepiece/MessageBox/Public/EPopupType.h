#pragma once

DECLARE_DELEGATE(FOnMsgBoxOkDelegate);
DECLARE_DELEGATE(FOnMsgBoxCancelDelegate);

/**
 * @brief 팝업 타입 정의
 *
 * 게임 내 모든 팝업 타입을 정의합니다.
 * 새로운 팝업 추가 시 이 Enum에 타입을 추가하세요.
 */
UENUM(Blueprintable)
enum class EPopupType : uint8
{
	// 메시지 박스
	MsgBox				UMETA(DisplayName = "Message Box"),

	// 입력 팝업
	InputMsg_Register	UMETA(DisplayName = "Register"),
	InputMsg_Login		UMETA(DisplayName = "Login"),
};
