// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


/**
 * @file FHttpMultipartFormData.cpp
 * @brief FHttpMultipartFormData의 동작을 구현합니다.
 */
#include "FHttpMultipartFormData.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FHttpMultipartFormData::FHttpMultipartFormData(EFormDataType Type)
	: FormDataType(Type)
{
	Reset();
}

void FHttpMultipartFormData::Reset()
{
	TextParts.Reset();
	FileParts.Reset();
	BuiltBody.Reset();

	// Multipart일 때만 Boundary 생성
	if (FormDataType == EFormDataType::Multipart)
	{
		// 어떤 고유한 토큰이든 괜찮습니다. 디버깅을 위해 읽기 쉽게 유지합니다.
		Boundary = TEXT("----UE_Multipart_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
	}
}

void FHttpMultipartFormData::AddText(const FString& FieldName, const FString& Value)
{
	TextParts.Add({ FieldName, Value });
}

bool FHttpMultipartFormData::AddFile(const FString& FieldName, const FString& FilePath, const FString& InMimeType)
{
	TArray<uint8> FileBytes;
	if (!FFileHelper::LoadFileToArray(FileBytes, *FilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("[HttpMultipartFormData] Failed to load file: %s"), *FilePath);
		return false;
	}

	FString FileNameOnly = FPaths::GetCleanFilename(FilePath);
	FString Mime = InMimeType.IsEmpty() ? DetectMimeFromExtension(FileNameOnly) : InMimeType;

	FileParts.Add({ FieldName, FileNameOnly, Mime, MoveTemp(FileBytes) });
	return true;
}

void FHttpMultipartFormData::SetupHttpRequest(const TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request)
{
	BuildBody();

	// FormDataType에 따라 다른 Content-Type 설정
	if (FormDataType == EFormDataType::FormUrlEncoded)
	{
		Request->SetHeader(TEXT("Content-Type"),
			TEXT("application/x-www-form-urlencoded"));
	}
	else // Multipart
	{
		// 경계를 포함한 Content-Type
		Request->SetHeader(TEXT("Content-Type"),
			FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary));
	}

	// 참고: Accept 헤더는 호출자가 설정해야 합니다. 많은 STT 엔드포인트는 JSON을 기대합니다.
	// Request->SetHeader(TEXT("Accept"), TEXT("application/json"));

	Request->SetContent(BuiltBody);
}

void FHttpMultipartFormData::BuildBody()
{
	BuiltBody.Reset();

	if (FormDataType == EFormDataType::FormUrlEncoded)
	{
		// application/x-www-form-urlencoded: key1=value1&key2=value2
		TArray<FString> Pairs;
		for (const FTextPart& P : TextParts)
		{
			FString EncodedKey = FGenericPlatformHttp::UrlEncode(P.Name);
			FString EncodedValue = FGenericPlatformHttp::UrlEncode(P.Value);
			Pairs.Add(FString::Printf(TEXT("%s=%s"), *EncodedKey, *EncodedValue));
		}

		FString Body = FString::Join(Pairs, TEXT("&"));
		AppendUtf8(BuiltBody, Body);
	}
	else // Multipart
	{
		auto Add = [](TArray<uint8>& Dest, const FString& S)
		{
			AppendUtf8(Dest, S);
		};

		const FString LineEnd = TEXT("\r\n");
		const FString BoundaryLine = TEXT("--") + Boundary;

		// 텍스트 파트
		for (const FTextPart& P : TextParts)
		{
			Add(BuiltBody, BoundaryLine + LineEnd);
			Add(BuiltBody, FString::Printf(
				TEXT("Content-Disposition: form-data; name=\"%s\"%s"),
				*P.Name, *LineEnd));
			Add(BuiltBody, LineEnd);          // 헤더와 값 사이의 빈 줄
			Add(BuiltBody, P.Value + LineEnd);
		}

		// 파일 파트
		for (const FFilePart& P : FileParts)
		{
			Add(BuiltBody, BoundaryLine + LineEnd);
			Add(BuiltBody, FString::Printf(
				TEXT("Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"%s"),
				*P.Name, *P.FileName, *LineEnd));
			Add(BuiltBody, FString::Printf(
				TEXT("Content-Type: %s%s"),
				*P.MimeType, *LineEnd));
			Add(BuiltBody, LineEnd);          // 바이너리 데이터 앞의 빈 줄

			// 바이너리 데이터
			BuiltBody.Append(P.Data);

			// 파일 내용 뒤의 CRLF
			AppendUtf8(BuiltBody, LineEnd);
		}

		// 닫는 경계
		Add(BuiltBody, BoundaryLine + TEXT("--") + LineEnd);
	}
}

void FHttpMultipartFormData::AppendUtf8(TArray<uint8>& Dest, const FString& Str)
{
	FTCHARToUTF8 Conv(*Str);
	Dest.Append(reinterpret_cast<const uint8*>(Conv.Get()), Conv.Length());
}

FString FHttpMultipartFormData::DetectMimeFromExtension(const FString& FileName)
{
	const FString Ext = FPaths::GetExtension(FileName, /*bIncludeDot*/ false).ToLower();

	// 일반적인 오디오 타입
	if (Ext == TEXT("mp3"))  return TEXT("audio/mpeg");
	if (Ext == TEXT("wav"))  return TEXT("audio/wav");
	if (Ext == TEXT("ogg"))  return TEXT("audio/ogg");
	if (Ext == TEXT("flac")) return TEXT("audio/flac");
	if (Ext == TEXT("m4a"))  return TEXT("audio/mp4");
	// 대체 타입
	if (Ext == TEXT("txt"))  return TEXT("text/plain");
	if (Ext == TEXT("json")) return TEXT("application/json");

	// 기본 타입
	return TEXT("application/octet-stream");
}