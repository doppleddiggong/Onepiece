// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/// @file NetworkData.h
/// @brief 네트워크 요청과 응답에 사용되는 구조체 및 설정을 정의합니다.
#pragma once

#include "CoreMinimal.h"
#include "UCustomNetworkSettings.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Templates/SharedPointer.h"
#include "FResultStatData.h"
#include "NetworkData.generated.h"

// =================================================================================
// Network Configuration
// 서버 주소와 포트를 설정합니다.
// =================================================================================
namespace NetworkConfig
{
    /// @brief 현재 서버 모드에 맞는 HTTP 엔드포인트 URL을 생성합니다.
    /// @param Endpoint [in] 호출할 상대 경로입니다.
    /// @return 모드와 포트가 적용된 전체 URL입니다.
    static FString GetFullUrl(const FString& Endpoint)
    {
        const EServerMode Mode = UCustomNetworkSettings::GetCurrentServerMode();
        const FServerConfig& Config = GetDefault<UCustomNetworkSettings>()->GetConfig(Mode);
        return Config.GetFullUrl(Endpoint);
    }

	/// @brief Query 파라미터가 포함된 전체 URL을 반환합니다.
	///        파라미터가 없으면 기본 URL만 반환합니다.
	static FString GetFullUrlWithQuery(
		const FString& Endpoint,
		const TMap<FString, FString>& QueryParams)
    {
    	FString BaseUrl = GetFullUrl(Endpoint);

    	if (QueryParams.Num() == 0)
    		return BaseUrl;

    	FString QueryString;
    	bool bFirst = true;

    	for (const auto& Pair : QueryParams)
    	{
    		if (Pair.Key.IsEmpty() || Pair.Value.IsEmpty())
    			continue;
    		
    		FString EncodedKey   = FGenericPlatformHttp::UrlEncode(Pair.Key);
    		FString EncodedValue = FGenericPlatformHttp::UrlEncode(Pair.Value);

    		if (bFirst)
    		{
    			QueryString += FString::Printf(TEXT("?%s=%s"), *EncodedKey, *EncodedValue);
    			bFirst = false;
    		}
    		else
    		{
    			QueryString += FString::Printf(TEXT("&%s=%s"), *EncodedKey, *EncodedValue);
    		}
    	}

    	return BaseUrl + QueryString;
    }

    /// @brief 현재 서버 모드에서 사용할 WebSocket 주소를 반환합니다.
    /// @return ws(s):// 형식의 소켓 접속 URL입니다.
    static FString GetSocketURL()
    {
        const EServerMode Mode = UCustomNetworkSettings::GetCurrentServerMode();
        const FServerConfig& Config = GetDefault<UCustomNetworkSettings>()->GetConfig(Mode);
        return Config.WebSocketUrl;
    }
}

namespace RequestAPI
{
    /// @brief 서버 상태 확인 엔드포인트입니다.
    static FString Health = FString("/health");


    static FString users_register = FString("/users/register");
    static FString users_token = FString("/users/token");
    static FString users_me = FString("/users/me");
	
    /// @brief Write 답변 제출 엔드포인트입니다. POST /writes/submit
    static FString writes_submit = FString("/writes/submit");

	static FString listenings_audio = FString("/listenings/audios");
	static FString speakings_judes = FString("/speakings/judges");
	static FString speakings_result = FString("/speakings/evaluate");

	static FString interview_hello = FString("/interview/hello");
	static FString interview_answer = FString("/interview/answer/post");

	/// @brief Scenario 조회 엔드포인트입니다.
	static FString scenario = FString("/scenario/stages/current");

	static FString read_result = FString("/scenario/stage/result/post");
	static FString listen_result = FString("/scenario/stage/result/post");
	static FString speak_result = FString("/scenario/stage/result/post");
	static FString wriite_result = FString("/scenario/stage/result/post");

	/// @brief Evaluation 결과 조회 엔드포인트입니다. GET /evaluations/rooms/{room_id}
	static FString evaluations_rooms = FString("/evaluations/rooms");

	/// @brief Chat 답변 요청 엔드포인트입니다. POST /chats/answers
	static FString chats_answers = FString("/chats/answers");
}


USTRUCT(BlueprintType)
struct FInterviewQuestionData
{
	GENERATED_BODY()

	// id : 126
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Id = 0;

	// type_code : 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TypeCode = 0;

	// "eng":"What do you do on weekends?"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Eng;

	// "kor":"주말에 보통 무엇을 합니까?"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Kor;

	// "eng_key":"weekends"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EngKey;

	// "kor_key":"주말"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString KorKey;

	// "created_at":"2025-11-28T15:51:30.017204"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CreatedAt;
};

/// @brief Read 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FInterviewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FInterviewQuestionData> Question;
};



// =================================================================================
// Common Data Structures
// =================================================================================

/// @brief 플레이어 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 UserIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	FString Nickname;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 Gold = 0;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 Character = -1;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 CharacterColor = -1;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	FString UnlockEquip;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	bool bLogin = false;
};

/// @brief 단어 데이터 구조체입니다.
USTRUCT(BlueprintType)
struct FPhonemeData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString Kor;

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString Pronunciation;

	FString ToRichTextString(int32 Index) const;
};

/// @brief 단어 데이터 구조체입니다.
USTRUCT(BlueprintType)
struct FWordData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString Kor;

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString Eng;

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString Pronunciation;

	TArray<FPhonemeData> GetPhonemeData() const;	
};

// =================================================================================
// Read Quest Structures
// =================================================================================

/// @brief Read 퀘스트 타겟 데이터입니다.
USTRUCT(BlueprintType)
struct FReadTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FindSymbol;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FindColor;
};

/// @brief Read 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestReadInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 ReadIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FReadTargetData> QuestTarget;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 CorrectAnswerIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FWordData WordData1;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FWordData WordData2;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FullData;
};

// =================================================================================
// Listen Quest Structures
// =================================================================================

/// @brief Listen 퀘스트 타겟 데이터입니다.
USTRUCT(BlueprintType)
struct FListenTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FindTarget1;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FindTarget2;
};

/// @brief Listen 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestListenInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 ListenIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FListenTargetData> QuestTarget;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 CorrectAnswerIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FWordData WordData1;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FWordData WordData2;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	FString FullData;
};

// =================================================================================
// Write Quest Request Structures
// =================================================================================

/// @brief Write 질문 구조체입니다.
USTRUCT(BlueprintType)
struct FWriteWordData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString kor;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString eng;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString pronunciation;
};

/// @brief Write 질문 및 정답 구조체입니다.
USTRUCT(BlueprintType)
struct FWriteQuestionData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	int32 Id;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FWriteWordData word_data;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString answer;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString answer_kor;
};

/// @brief Write Submit 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseWriteQuestionDelegate, FQuestWriteInfo&, bool);
/// @brief Write 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestWriteInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Write")
	int32 user_id;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	TArray<FWriteQuestionData> question;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	bool bIsValid = false;
	
	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
	
	/// @brief 구조체 값이 유효한지 여부를 반환합니다.
	bool IsValid() const;
};

// =================================================================================
// Speak Quest Structures
// =================================================================================

/// @brief Speak 퀘스트 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FQuestSpeakInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 SpeakIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FString> AnswerPlayer;

	UPROPERTY(BlueprintReadWrite, Category = "KLingo")
	TArray<FString> QuestTarget;
};

// =================================================================================
// Read Quest Role and Result Structures
// =================================================================================

/// @brief Read 퀘스트의 플레이어 결과 기록입니다.
USTRUCT(BlueprintType)
struct FReadQuestResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	bool bSuccess = false;  // 성공 여부

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	float RemainTime = 0.f;  // 남은 시간

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	int32 AttemptCount = 0;  // 시도 횟수

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	FString SelectedSymbol;  // 선택한 심볼

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	FString SelectedColor;   // 선택한 색상
};

// =================================================================================
// Response Structures
// =================================================================================
/// @brief 헬스 체크 응답을 전달하는 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseHealthDelegate, FResponseHealth&, bool);
USTRUCT(BlueprintType)
struct FResponseHealth
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 status = 0;

    /// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
    void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

    /// @brief 디버그 로그에 응답 내용을 출력합니다.
    void PrintData();
};


DECLARE_DELEGATE_TwoParams(FResponseUserRegisterDelegate, FResponseUserRegister&, bool);
USTRUCT(BlueprintType)
struct FResponseUserRegister
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 id = 0;

	UPROPERTY(BlueprintReadWrite)
	FString username;

	UPROPERTY(BlueprintReadWrite)
	FString email;

	UPROPERTY(BlueprintReadWrite)
	bool is_active;
	
	/// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseUserTokenDelegate, FResponseUserToken&, bool);
USTRUCT(BlueprintType)
struct FResponseUserToken
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Token")
	FString access_token;

	/// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseUserMeDelegate, FResponseUserMe&, bool);
USTRUCT(BlueprintType)
struct FResponseUserMe
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Me")
	int id;

	UPROPERTY(BlueprintReadWrite, Category = "Me")
	FString username;

	UPROPERTY(BlueprintReadWrite, Category = "Me")
	FString fullname;

	UPROPERTY(BlueprintReadWrite, Category = "Me")
	bool is_active;

	UPROPERTY(BlueprintReadWrite, Category = "Me")
	FString my_avatar;

	UPROPERTY(BlueprintReadWrite, Category = "Me")
	FString my_color;
	
	/// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseUserHostDelegate, FResponseUserHost&, bool);
USTRUCT(BlueprintType)
struct FResponseUserHost
{
	GENERATED_BODY()

	/// @brief HTTP 응답을 파싱해 상태 정보를 갱신합니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


// =================================================================================
// Scenario API Structures
// =================================================================================

/// @brief 단어 정보 구조체                                                                                                                                                                                                       
USTRUCT(BlueprintType)
struct FWordInfo                                                                                                                                                                                                                  
{                                                                                                                                                                                                                                 
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString name;

	UPROPERTY(BlueprintReadWrite, Category = "Word")
	FString code;

    /** 랜덤 색상 선택 - CSV 기반 하드코딩 */
    static FWordInfo GetRandomColor()
    {
        static const TArray<FWordInfo> ColorPool = {
            { TEXT("빨강"),   TEXT("1") },
            { TEXT("파랑"),   TEXT("2") },
            { TEXT("노랑"),   TEXT("3") },
            { TEXT("초록"),   TEXT("4") },
            { TEXT("검정"),   TEXT("5") },
            { TEXT("흰색"),   TEXT("6") },
            { TEXT("회색"),   TEXT("7") },
            { TEXT("주황"),   TEXT("8") },
            { TEXT("분홍"),   TEXT("9") },
            { TEXT("갈색"),   TEXT("10") },
            { TEXT("남색"),   TEXT("11") },
            { TEXT("보라색"), TEXT("12") },
            { TEXT("금색"),   TEXT("13") },
            { TEXT("은색"),   TEXT("14") },
            { TEXT("살구색"), TEXT("15") },
            { TEXT("하늘색"), TEXT("16") },
            { TEXT("자주색"), TEXT("17") },
            { TEXT("청록색"), TEXT("18") },
            { TEXT("황토색"), TEXT("19") },
            { TEXT("진홍색"), TEXT("20") },
            { TEXT("군청색"), TEXT("21") },
            { TEXT("연두색"), TEXT("22") },
            { TEXT("와인색"), TEXT("23") },
            { TEXT("베이지색"), TEXT("24") }
        };

        const int Index = FMath::RandRange(0, ColorPool.Num() - 1);
        return ColorPool[Index];
    }


    /** 랜덤 동물 선택 - CSV 기반 하드코딩 */
    static FWordInfo GetRandomAnimal()
    {
        static const TArray<FWordInfo> AnimalPool = {
            { TEXT("개"),      TEXT("1") },
            { TEXT("고양이"),  TEXT("2") },
            { TEXT("새"),      TEXT("3") },
            { TEXT("물고기"),  TEXT("4") },
            { TEXT("소"),      TEXT("5") },
            { TEXT("닭"),      TEXT("6") },
            { TEXT("돼지"),    TEXT("7") },
            { TEXT("토끼"),    TEXT("8") },
            { TEXT("말"),      TEXT("9") },
            { TEXT("양"),      TEXT("10") },
            { TEXT("사슴"),    TEXT("11") },
            { TEXT("호랑이"),  TEXT("12") },
            { TEXT("곰"),      TEXT("13") },
            { TEXT("여우"),    TEXT("14") },
            { TEXT("원숭이"),  TEXT("15") },
            { TEXT("펭귄"),    TEXT("16") },
            { TEXT("기린"),    TEXT("17") },
            { TEXT("코끼리"),  TEXT("18") },
            { TEXT("치타"),    TEXT("19") },
            { TEXT("물개"),    TEXT("20") },
            { TEXT("하마"),    TEXT("21") },
            { TEXT("낙타"),    TEXT("22") },
            { TEXT("돌고래"),  TEXT("23") },
            { TEXT("박쥐"),    TEXT("24") }
        };

        const int Index = FMath::RandRange(0, AnimalPool.Num() - 1);
        return AnimalPool[Index];
    }

    /** TSet에서 사용하기 위한 비교 연산자 */
    bool operator==(const FWordInfo& Other) const
    {
        return name == Other.name && code == Other.code;
    }

    bool operator!=(const FWordInfo& Other) const
    {
        return !(*this == Other);
    }
};

/** TSet에서 사용하기 위한 해시 함수 */
inline uint32 GetTypeHash(const FWordInfo& Info)
{
    return HashCombine(GetTypeHash(Info.name), GetTypeHash(Info.code));
}

/// @brief Scenario 타겟 데이터입니다.
USTRUCT(BlueprintType)
struct FScenarioTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	FWordInfo word1;

	UPROPERTY(BlueprintReadWrite, Category = "Scenario")
	FWordInfo word2;
};

// =================================================================================
// Write Quest Response API Structures
// =================================================================================

USTRUCT(BlueprintType)
struct FWriteDisplay
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	bool is_pass = false;

	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString message;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString correction;
};

USTRUCT(BlueprintType)
struct FWriteRecord
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	int32 score;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString target;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString input;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString stage;
};

USTRUCT(BlueprintType)
struct FResponseWriteData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FWriteDisplay display;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FWriteRecord record;
};

/// @brief Write Submit 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseWriteSubmitDelegate, FResponseWriteSubmit&, bool);
/// @brief Write Submit 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseWriteSubmit
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Write")
	TArray<FResponseWriteData> ResponseWriteDataArray;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};

USTRUCT(BlueprintType)
struct FRequestWriteResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 room_id;
	
	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 user_id;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 scenario_id;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 stage_type;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 state_type;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 result_time;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	TArray<int32> wrong_idx;
	
	/// @brief 구조체를 JSON 문자열로 변환합니다.
	bool ToJsonString(FString& OutJson) const;
};

/// @brief Write Scenario 최종 결과 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseWriteResultDelegate, FResponseWriteResult&, bool);
/// @brief Write 최종 결과 점수 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseWriteScores
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	int32 score;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString desc;
};

USTRUCT(BlueprintType)
struct FResponseWriteResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	FString grade;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	int32 average_score;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	int32 top_percent;
	
	UPROPERTY(BlueprintReadWrite, Category = "Write")
	TArray<FResponseWriteScores> scores;
	
	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);
};

// =================================================================================
// Speaking Questions API Structures (Updated)
// =================================================================================

/// @brief Speaking Questions 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseListenAudioDelegate, FResponseListenAudio&, bool);
USTRUCT(BlueprintType)
struct FResponseListenAudio
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Speaking")
	FString audio_text;

	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> audio_base64;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};



// =================================================================================
// Speaking Questions API Structures (Updated)
// =================================================================================

/// @brief Speaking Questions 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseSpeakingJudesDelegate, FResponseSpeakingJudes&, bool);
/// @brief Speaking Questions 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseSpeakingJudes
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Speaking")
	int grammar_score;
		
	UPROPERTY(BlueprintReadWrite, Category = "Speaking")
	int context_score;

	UPROPERTY(BlueprintReadWrite, Category = "Speaking")
	int final_overall_score;

	UPROPERTY(BlueprintReadWrite, Category = "Speaking")
	FString final_feedback;
	
	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;

	TArray<FResultStatData> GetResultStatData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseInterviewHelloDelegate, FResponseInterviewHello&, bool);
USTRUCT(BlueprintType)
struct FResponseInterviewHello
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Interview")
	TArray<FInterviewQuestionData> Questions;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};

USTRUCT(BlueprintType)
struct FInterviewAnswerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int interview_id;

	UPROPERTY(BlueprintReadWrite)
	FString answer;

	UPROPERTY(BlueprintReadWrite)
	int user_id;
};

USTRUCT(BlueprintType)
struct FRequestInterviewAnswer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Interview")
	TArray<FInterviewAnswerData> answer;

	/// @brief 구조체를 JSON 문자열로 변환합니다.
	bool ToJsonString(FString& OutJson) const;
};

DECLARE_DELEGATE_TwoParams(FResponseInterviewAnswerDelegate, FResponseInterviewAnswer&, bool);
USTRUCT(BlueprintType)
struct FResponseInterviewAnswer
{
	GENERATED_BODY()

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};

DECLARE_DELEGATE_TwoParams(FResponseReadScenarioDelegate, FResponseReadScenario&, bool);
USTRUCT(BlueprintType)
struct FResponseReadScenario
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Read")
	int32 index = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Read")
	int32 dificulity = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Read")
	int32 room_id = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Read")
	TArray<FScenarioTargetData> target_data;

	UPROPERTY(BlueprintReadWrite, Category = "Read")
	int32 correct_answer_index = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Read")
	FWordData word_data1;

	UPROPERTY(BlueprintReadWrite, Category = "Read")
	FWordData word_data2;

	UPROPERTY(BlueprintReadWrite, Category = "Read")
	FWordData full_data;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;

	TArray<FString> GetWord1List() const;
	TArray<FString> GetWord2List() const;

	FScenarioTargetData GetCorrectAnswerData() const;
};

USTRUCT(BlueprintType)
struct FRequestReadResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 room_id;
	
	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 user_id;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 scenario_id;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 stage_type;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 state_type;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 result_time;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	TArray<int32> wrong_idx;
	
	/// @brief 구조체를 JSON 문자열로 변환합니다.
	bool ToJsonString(FString& OutJson) const;
};


DECLARE_DELEGATE_TwoParams(FResponseReadResultDelegate, FResponseReadResult&, bool);
USTRUCT(BlueprintType)
struct FResponseReadResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	FString grade;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 average_score;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	float top_percent;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


DECLARE_DELEGATE_TwoParams(FResponseListenScenarioDelegate, FResponseListenScenario&, bool);
USTRUCT(BlueprintType)
struct FResponseListenScenario
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Listen")
	int32 index = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Listen")
	int32 dificulity = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Listen")
	int32 room_id = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Listen")
	TArray<FScenarioTargetData> target_data;

	UPROPERTY(BlueprintReadWrite, Category = "Listen")
	int32 correct_answer_index = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Listen")
	FWordData word_data1;

	UPROPERTY(BlueprintReadWrite, Category = "Listen")
	FWordData word_data2;

	UPROPERTY(BlueprintReadWrite, Category = "Listen")
	FWordData full_data;

	UPROPERTY(BlueprintReadWrite, NotReplicated, Category = "Listen")
	TArray<uint8> voice_data;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;

	TArray<FString> GetWord1List() const;
	TArray<FString> GetWord2List() const;

	FScenarioTargetData GetCorrectAnswerData() const;
};

USTRUCT(BlueprintType)
struct FRequestListenResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 room_id;
	
	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 user_id;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 scenario_id;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 stage_type;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 state_type;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 result_time;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	TArray<int32> wrong_idx;
	
	/// @brief 구조체를 JSON 문자열로 변환합니다.
	bool ToJsonString(FString& OutJson) const;
};

DECLARE_DELEGATE_TwoParams(FResponseListenResultDelegate, FResponseListenResult&, bool);
USTRUCT(BlueprintType)
struct FResponseListenResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	FString grade;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 average_score;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	float top_percent;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


// =================================================================================
// Speak Quest Structures (KLingo SpeakStage)
// =================================================================================

/// @brief SpeakQuest 오디오 질문 데이터 구조체입니다.
/// @note API의 audio 배열 항목을 저장합니다.
USTRUCT(BlueprintType)
struct FSpeakStageQuestion
{
	GENERATED_BODY()

	/// @brief 한국어 질문
	UPROPERTY(BlueprintReadWrite, Category = "SpeakQuest")
	FString kor;

	/// @brief 영어 질문
	UPROPERTY(BlueprintReadWrite, Category = "SpeakQuest")
	FString eng;

	/// @brief 발음 가이드
	UPROPERTY(BlueprintReadWrite, Category = "SpeakQuest")
	FString pronunciation;

	/// @brief 음성 데이터 Base64
	UPROPERTY(BlueprintReadWrite, NotReplicated, Category = "SpeakQuest")
	TArray<uint8> voice_data;

	FString GetQuestionMessage() const;
};

// =================================================================================
// Speak Scenario API Structures
// =================================================================================

/// @brief Speak 시나리오 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseSpeakScenarioDelegate, FResponseSpeakScenario&, bool);
/// @brief Speak 시나리오 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseSpeakScenario
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Speak")
	int32 index = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Speak")
	int32 difficulty = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Speak")
	int32 room_id = 0;

	/// @brief 오디오 질문 목록
	UPROPERTY(BlueprintReadWrite, Category = "Speak")
	TArray<FSpeakStageQuestion> speak_quest_data;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


USTRUCT(BlueprintType)
struct FRequestSpeakResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 room_id;
	
	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 user_id;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 scenario_id;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 stage_type;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 state_type;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 result_time;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	TArray<int32> wrong_idx;
	
	/// @brief 구조체를 JSON 문자열로 변환합니다.
	bool ToJsonString(FString& OutJson) const;
};

/// @brief Speak 퀘스트의 개별 점수 상세 정보를 담는 구조체입니다.
USTRUCT(BlueprintType)
struct FSpeakScoreDetail
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 score = 0;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	FString desc;
};


DECLARE_DELEGATE_TwoParams(FResponseSpeakResultDelegate, FResponseSpeakResult&, bool);
USTRUCT(BlueprintType)
struct FResponseSpeakResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	FString grade;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	int32 average_score;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	float top_percent;

	UPROPERTY(BlueprintReadWrite, Category = "QuestResult")
	TArray<FSpeakScoreDetail> scores;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;

	TArray<FResultStatData> GetResultStatData() const;
};


// =================================================================================
// Evaluation API Structures
// =================================================================================

/// @brief 시나리오 타입 열거형
UENUM(BlueprintType)
enum class EScenarioType : uint8
{
	READING UMETA(DisplayName = "Reading"),
	LISTENING UMETA(DisplayName = "Listening"),
	WRITING UMETA(DisplayName = "Writing"),
	SPEAKING UMETA(DisplayName = "Speaking")
};

/// @brief 피드백 요약 구조체입니다.
USTRUCT(BlueprintType)
struct FFeedbackSummary
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FString title;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FString message;
};

/// @brief 시나리오별 결과 구조체입니다.
USTRUCT(BlueprintType)
struct FScenarioResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	EScenarioType scenario_type;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FString display_name;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	int32 final_score = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FString grade;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FFeedbackSummary feedback_summary;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FString action_item;
};

/// @brief 전체 결과 구조체입니다.
USTRUCT(BlueprintType)
struct FTotalResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	int32 final_score = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FString grade;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FString feedback_summary;
};

/// @brief Evaluation 결과 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseEvaluationResultDelegate, FResponseEvaluationResult&, bool);
/// @brief Evaluation 결과 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseEvaluationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	FTotalResult total_result;

	UPROPERTY(BlueprintReadWrite, Category = "Evaluation")
	TArray<FScenarioResult> scenario_results;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};


// =================================================================================
// Chat Answers API Structures
// =================================================================================

/// @brief Chat Answers 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseChatAnswersDelegate, FResponseChatAnswers&, bool);
/// @brief Chat Answers 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseChatAnswers
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Chat")
	FString answer;

	/// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
	void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

	/// @brief 디버그 로그에 응답 내용을 출력합니다.
	void PrintData() const;
};