// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

/**
 * @file FHttpMultipartFormData.h
 * @brief FHttpMultipartFormData 클래스를 선언합니다.
 */
#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

/// @brief HTTP 폼 데이터 전송 방식을 정의합니다.
enum class EFormDataType : uint8
{
	/// @brief multipart/form-data (파일 업로드용)
	Multipart,
    MultipartArray,
	/// @brief application/x-www-form-urlencoded (OAuth2 등)
	FormUrlEncoded
};

/// @brief 언리얼 엔진에 기본 제공되지 않는 multipart/form-data 빌더의 경량 구현입니다.
/// @details
/// - 경계(boundary)를 자동으로 생성합니다.
/// - 텍스트 및 파일 필드를 추가할 수 있습니다.
/// - RFC 7578 규격에 맞는 HTTP 본문을 생성합니다.
class FHttpMultipartFormData
{
public:
    /// @brief FormData 객체를 생성합니다.
    /// @param Type [in] 전송 방식 (기본값: Multipart)
    FHttpMultipartFormData(EFormDataType Type = EFormDataType::Multipart);

    /// @brief 간단한 텍스트 필드를 추가합니다.
    /// @param FieldName [in] 폼 필드 이름입니다.
    /// @param Value [in] 필드에 저장할 문자열 값입니다.
    void AddText(const FString& FieldName, const FString& Value);

    /// @brief 문자열 필드를 추가하는 편의 래퍼입니다.
    FORCEINLINE void AddStringField(const FString& FieldName, const FString& Value)
    {
        AddText(FieldName, Value);
    }

    /// @brief 파일 데이터를 multipart 본문에 추가합니다.
    /// @param FieldName [in] 폼 필드 이름입니다.
    /// @param FilePath [in] 읽어올 파일 경로입니다.
    /// @param MimeType [in] 명시적 MIME 타입(비어있으면 자동 추론)입니다.
    /// @return 파일 로드 및 추가에 성공하면 true입니다.
    bool AddFile(const FString& FieldName, const FString& FilePath, const FString& MimeType = TEXT(""));

    /// @brief 모든 파트를 제거하고 경계를 재생성합니다.
    void Reset();

    /// @brief HTTP 요청에 Content-Type 헤더와 본문을 설정합니다.
    /// @param Request [in] 전송할 HTTP 요청 객체입니다.
    void SetupHttpRequest(const TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request);

    /// @brief 현재 사용 중인 경계 문자열입니다.
    const FString& GetBoundary() const { return Boundary; }

    /// @brief 직렬화된 multipart 본문 데이터입니다.
    const TArray<uint8>& GetBuiltBody() const { return BuiltBody; }

private:
    struct FTextPart
    {
        FString Name;
        FString Value;
    };

    struct FFilePart
    {
        FString Name;
        FString FileName;
        FString MimeType;
        TArray<uint8> Data;
    };

private:
    /// @brief 파일 확장자를 기반으로 MIME 타입을 추론합니다.
    static FString DetectMimeFromExtension(const FString& FileName);

    /// @brief 문자열을 UTF-8로 변환해 버퍼에 추가합니다.
    static void AppendUtf8(TArray<uint8>& Dest, const FString& Str);

    /// @brief 등록된 파트를 기반으로 HTTP 본문을 구성합니다.
    void BuildBody();

private:
    EFormDataType FormDataType;
    FString Boundary;
    TArray<FTextPart> TextParts;
    TArray<FFilePart> FileParts;
    TArray<uint8> BuiltBody;
};
