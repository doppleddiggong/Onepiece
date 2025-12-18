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
	Register			UMETA(DisplayName = "Register"),
	Login				UMETA(DisplayName = "Login"),

	ReadQuest			UMETA(DisplayName = "ReadQuest"),
	Result				UMETA(DisplayName = "Result"),
	Interview			UMETA(DisplayName = "Interview"),
	Questionnaire		UMETA(DisplayName = "Questionnaire"),
	WriteBoard			UMETA(DisplayName = "WriteBoard"),
	QuestionnaireResult UMETA(DisplayName = "QuestionnaireResult"),

	Word				UMETA(DisplayName = "Word"),
	InterviewHello		UMETA(DisplayName = "InterviewHello"),

	SpeakQuest			UMETA(DisplayName = "SpeakQuest"),
	SpeakQuestJudes		UMETA(DisplayName = "SpeakQuestJudes"),
	SpeakResult			UMETA(DisplayName = "SpeakResult"),

	Evaluation			UMETA(DisplayName = "Evaluation"),

	AskTutorial			UMETA(DisplayName = "AskTutorial"),

	LevelSelect			UMETA(DisplayName = "LevelSelect"),
};

UENUM(BlueprintType)
enum class EMsgBoxType : uint8
{
	OK,
	OK_CANCEL,
};