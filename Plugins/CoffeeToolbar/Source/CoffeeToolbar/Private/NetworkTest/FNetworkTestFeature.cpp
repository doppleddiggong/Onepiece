#include "NetworkTest/FNetworkTestFeature.h"
#include "Settings/UToolbarSettings.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "GenericPlatform/GenericPlatformHttp.h"

#define LOCTEXT_NAMESPACE "FNetworkTestFeature"

DEFINE_LOG_CATEGORY_STATIC(LogCoffeeToolbarNetworkTest, Log, All);

// 유틸: URL 합성 (절대 URL이면 그대로, 아니면 BaseUrl + Endpoint)
static FString BuildFullUrl(const FString BaseUrl, const FString& Input)
{
	if (Input.StartsWith(TEXT("http://")) || Input.StartsWith(TEXT("https://")))
	{
		return Input;
	}

	if (BaseUrl.IsEmpty())
	{
		return Input;
	}

	// 슬래시 중복 방지
	if (BaseUrl.EndsWith(TEXT("/")) && Input.StartsWith(TEXT("/")))
	{
		return BaseUrl.LeftChop(1) + Input;
	}
	if (!BaseUrl.EndsWith(TEXT("/")) && !Input.StartsWith(TEXT("/")))
	{
		return BaseUrl + TEXT("/") + Input;
	}
	return BaseUrl + Input;
}

TSharedRef<SWidget> FNetworkTestFeature::GenerateNetworkTestMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	const UToolbarSettings* Settings = GetDefault<UToolbarSettings>();

	if (Settings && !Settings->NetworkTests.IsEmpty())
	{
		for (int32 i = 0; i < Settings->NetworkTests.Num(); ++i)
		{
			const FApiSendInfo& ApiInfo = Settings->NetworkTests[i];

			if (ApiInfo.Endpoint.IsEmpty())
			{
				continue;
			}

			FUIAction Action(FExecuteAction::CreateRaw(this, &FNetworkTestFeature::ExecuteApiRequest, i));

			const FString DisplayLabel = ApiInfo.Label.IsEmpty()
				? FString::Printf(TEXT("%s %s"),
					ApiInfo.Method == EApiHttpMethod::GET ? TEXT("GET") : TEXT("POST"),
					*ApiInfo.Endpoint)
				: ApiInfo.Label;

			MenuBuilder.AddMenuEntry(
				FText::FromString(DisplayLabel),
				FText::FromString(FString::Printf(TEXT("Execute %s %s"),
					ApiInfo.Method == EApiHttpMethod::GET ? TEXT("GET") : TEXT("POST"),
					*ApiInfo.Endpoint)),
				FSlateIcon(),
				Action
			);
		}
	}
	else
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("NoApisConfigured", "No APIs configured"),
			LOCTEXT("NoApisConfigured_Tooltip", "Add API tests in Project Settings > Plugins > Level Selector > Network Test"),
			FSlateIcon(),
			FUIAction()
		);
	}

	return MenuBuilder.MakeWidget();
}

void FNetworkTestFeature::ExecuteApiRequest(int32 ApiIndex)
{
	const UToolbarSettings* Settings = GetDefault<UToolbarSettings>();
	if (!Settings || ApiIndex < 0 || ApiIndex >= Settings->NetworkTests.Num())
	{
		UE_LOG(LogCoffeeToolbarNetworkTest, Error, TEXT("Invalid API index: %d"), ApiIndex);
		return;
	}

	const FString BaseUrl = Settings ? Settings->NetworkTestUrl : TEXT("http://127.0.0.1:8000");
	const FApiSendInfo& ApiInfo = Settings->NetworkTests[ApiIndex];
	const FString FullUrl = BuildFullUrl(BaseUrl, ApiInfo.Endpoint);

	// HTTP 요청 생성
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(FullUrl);
	Request->SetTimeout(60);
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));

	// 커스텀 헤더 추가
	for (const auto& HeaderPair : ApiInfo.Headers)
	{
		Request->SetHeader(HeaderPair.Key, HeaderPair.Value);
	}

	// 저장된 access_token을 Authorization 헤더로 자동 추가 (이미 설정되지 않은 경우)
	if (!ApiInfo.Headers.Contains(TEXT("Authorization")) && !Settings->LastAccessToken.IsEmpty())
	{
		const FString AuthHeaderValue = FString::Printf(TEXT("Bearer %s"), *Settings->LastAccessToken);
		Request->SetHeader(TEXT("Authorization"), AuthHeaderValue);

		UE_LOG(LogCoffeeToolbarNetworkTest, Log,
			TEXT("[Network Test] Auto-applied Authorization header from saved token"));
	}

	// POST 바디 저장용
	FString JsonBodyString;

	// 메서드별 설정
	if (ApiInfo.Method == EApiHttpMethod::GET)
	{
		Request->SetVerb(TEXT("GET"));
	}
	else // POST
	{
		Request->SetVerb(TEXT("POST"));

		// ContentType에 따라 다르게 처리
		if (ApiInfo.ContentType == EApiContentType::FormUrlEncoded)
		{
			// application/x-www-form-urlencoded
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

			if (ApiInfo.BodyParams.Num() > 0)
			{
				TArray<FString> FormPairs;
				for (const auto& Pair : ApiInfo.BodyParams)
				{
					FString EncodedKey = FGenericPlatformHttp::UrlEncode(Pair.Key);
					FString EncodedValue = FGenericPlatformHttp::UrlEncode(Pair.Value);
					FormPairs.Add(FString::Printf(TEXT("%s=%s"), *EncodedKey, *EncodedValue));
				}
				JsonBodyString = FString::Join(FormPairs, TEXT("&"));
				Request->SetContentAsString(JsonBodyString);
			}
		}
		else // JSON (기본값)
		{
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

			// BodyParams를 JSON으로 변환
			if (ApiInfo.BodyParams.Num() > 0)
			{
				TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
				for (const auto& Pair : ApiInfo.BodyParams)
				{
					JsonObject->SetStringField(Pair.Key, Pair.Value);
				}

				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonBodyString);
				FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

				Request->SetContentAsString(JsonBodyString);
			}
		}
	}

	// 응답 처리
	Request->OnProcessRequestComplete().BindLambda(
		[ApiInfo, Settings](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess) mutable
		{
			if (bSuccess && Res.IsValid())
			{
				const FString ResponseContent = Res->GetContentAsString();

				UE_LOG(LogCoffeeToolbarNetworkTest, Log,
					TEXT("[Network Test] %s %s -> %d\n[RES] %s"),
					*Req->GetVerb(),
					*Req->GetURL(),
					Res->GetResponseCode(),
					*ResponseContent);

				// access_token 추출 및 저장
				if (Res->GetResponseCode() == 200)
				{
					TSharedPtr<FJsonObject> JsonObject;
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

					if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
					{
						if (JsonObject->HasField(TEXT("access_token")))
						{
							const FString AccessToken = JsonObject->GetStringField(TEXT("access_token"));

							if (UToolbarSettings* MutableSettings = GetMutableDefault<UToolbarSettings>())
							{
								MutableSettings->LastAccessToken = AccessToken;
								MutableSettings->SaveConfig();

								UE_LOG(LogCoffeeToolbarNetworkTest, Log,
									TEXT("[Network Test] Saved access_token to config"));
							}
						}
					}
				}
			}
			else
			{
				const int32 Code = (Res.IsValid() ? Res->GetResponseCode() : -1);
				UE_LOG(LogCoffeeToolbarNetworkTest, Error,
					TEXT("[Network Test] Request failed. Code=%d URL=%s"),
					Code, Req.IsValid() ? *Req->GetURL() : TEXT("<null>"));
			}
		}
	);

	// 요청 로그 출력
	if (ApiInfo.Method == EApiHttpMethod::POST && !JsonBodyString.IsEmpty())
	{
		UE_LOG(LogCoffeeToolbarNetworkTest, Log,
			TEXT("[Network Test] POST %s\n[BODY] %s"),
			*FullUrl,
			*JsonBodyString);
	}
	else
	{
		UE_LOG(LogCoffeeToolbarNetworkTest, Log,
			TEXT("[Network Test] %s %s"),
			ApiInfo.Method == EApiHttpMethod::GET ? TEXT("GET") : TEXT("POST"),
			*FullUrl);
	}

	Request->ProcessRequest();
}

#undef LOCTEXT_NAMESPACE
