// Copyright (c) 2025 Doppleddiggong. All rights reserved.

/**
 * @file UCommonFunctionLibrary.cpp
 * @brief UCoffeeCommonUtil의 동작을 구현합니다.
 */
#include "UCommonFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/DateTime.h"
//
// void UCoffeeCommonUtil::TestULog()
// {
// 	int32 Number = 100;
// 	float Pi = 3.14f;
// 	int64 LongValue = 10000;
// 	double DoubleValue = 100000;
// 	bool IsGood = false;
// 	FString MyName = "MyName";
// 	
// 	// --- ULOG 기능 테스트 ---
// 	UE_LOG( LogTemp,  Warning, TEXT("Hello World"));
// 	UE_LOG( LogTemp, Warning, TEXT("Number : %d"), Number);
// 	// 결과: "9,999" (en-US 기준)
// 	FText LocalizedNumber = FText::AsNumber(Number);
// 	UE_LOG( LogTemp, Warning, TEXT("#,##0 : %s"), *LocalizedNumber.ToString());
// 	UE_LOG( LogTemp, Warning, TEXT("pi : %f"), Pi);
// 	UE_LOG( LogTemp, Warning, TEXT("longValue : %lld"), LongValue);
// 	UE_LOG( LogTemp, Warning, TEXT("doubleValue : %f"), DoubleValue);
// 	UE_LOG( LogTemp, Warning, TEXT("isGood : %d"), IsGood);
// 	UE_LOG( LogTemp, Warning, TEXT("myName : %s"), *MyName);
// 	UE_LOG( LogTemp, Warning, TEXT("myName : %s"), TEXT("배주백"));
// }
//
// void UCoffeeCommonUtil::TestInBound()
// {
// 	// --- CoffeeLibrary 기능 테스트 ---
// 	UE_LOG(LogTemp, Warning, TEXT("--- Testing CoffeeLibrary::CommonUtil::InBounds ---"));
//
// 	// MyArrayCount는 'InBounds' 함수를 테스트하기 위한 가상의 배열 크기입니다.
// 	constexpr int32 MyArrayCount = 5;
// 	constexpr int32 TestIndex1 = 3;  // 유효한 인덱스
// 	constexpr int32 TestIndex2 = 5;  // 유효하지 않은 인덱스
//
// 	const bool bIsIndex1InBounds = InBounds(TestIndex1, MyArrayCount);
// 	UE_LOG(LogTemp, Warning, TEXT("Is index %d in bounds [0..%d)? -> %s"),
// 		TestIndex1, MyArrayCount, bIsIndex1InBounds ? TEXT("True") : TEXT("False"));
//
// 	const bool bIsIndex2InBounds = InBounds(TestIndex2, MyArrayCount);
// 	UE_LOG(LogTemp, Warning, TEXT("Is index %d in bounds [0..%d)? -> %s"),
// 		TestIndex2, MyArrayCount, bIsIndex2InBounds ? TEXT("True") : TEXT("False"));
// }


FString UCommonFunctionLibrary::GererateMD5(const FString& InText)
{
	FTCHARToUTF8 UTF8Text(*InText);

	uint8 Digest[16];
	FMD5 Md5;
	Md5.Update((uint8*)UTF8Text.Get(), UTF8Text.Length());
	Md5.Final(Digest);

	FString HashString;
	for (uint8 B : Digest)
		HashString += FString::Printf(TEXT("%02x"), B);
	return HashString;
}

bool UCommonFunctionLibrary::InBounds(const int32 Index, const int32 Count)
{
	return (Index >= 0) && (Index < Count);
}

int32 UCommonFunctionLibrary::GetRandomIndex(const TArray<int32>& TargetArray, bool& bIsValid)
{
	if (TargetArray.Num() == 0)
	{
		bIsValid = false;
		return INDEX_NONE;
	}
	bIsValid = true;
	return TargetArray[FMath::RandRange(0, TargetArray.Num() - 1)];
}

UAnimMontage* UCommonFunctionLibrary::GetRandomMontage(const TArray<UAnimMontage*>& Montages)
{
	const int32 Num = Montages.Num();
	if (Num <= 0)
		return nullptr;

	const int32 Idx = FMath::RandRange(0, Num - 1);
	return Montages[Idx];
}

int64 UCommonFunctionLibrary::GetNowTimestamp()
{
	const auto DataTime = FDateTime::UtcNow();
	return DataTime.ToUnixTimestamp();
}

UMaterialInstanceDynamic* UCommonFunctionLibrary::GetOrCreateMID(
	UPrimitiveComponent* Target, int32 ElementIndex, FName OptionalName)
{
	if (!Target)
		return nullptr;

	const int32 Num = Target->GetNumMaterials();
	if (ElementIndex < 0 || ElementIndex >= Num)
		return nullptr;

	UMaterialInterface* CurMat = Target->GetMaterial(ElementIndex);
	if (!CurMat)
		return nullptr;

	if (UMaterialInstanceDynamic* AsMID = Cast<UMaterialInstanceDynamic>(CurMat))
		return AsMID;

	UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(CurMat, Target, OptionalName);
	if (NewMID)
		Target->SetMaterial(ElementIndex, NewMID);

	return NewMID;
}

void UCommonFunctionLibrary::PlayLocationSound(const AActor* Actor, USoundBase* Sound, const float RetriggerDelay)
{
	if (!IsValid(Actor) || !Sound)
		return;
	
	if (auto World = Actor->GetWorld())
	{
		if ( RetriggerDelay > 0.0f)
		{
			TWeakObjectPtr<const AActor> WeakActor = Actor;
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, [WeakActor, Sound]()
			{
				if (WeakActor.IsValid())
				{
					UGameplayStatics::PlaySoundAtLocation(WeakActor.Get(), Sound, WeakActor->GetActorLocation());
				}
			}, RetriggerDelay, false);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(Actor, Sound, Actor->GetActorLocation());
		}
	}
}

float UCommonFunctionLibrary::GetDistance(AActor* A, AActor* B)
{
	if ( !IsValid(A) || !IsValid(B) )
		return 0;

	return UKismetMathLibrary::Vector_Distance( A->GetActorLocation(), B->GetActorLocation() );
}

FString UCommonFunctionLibrary::RemoveLineBreaks(const FString& InText)
{
	FString CleanedText = InText;
	CleanedText.ReplaceInline(TEXT("\r\n"), TEXT(""));
	CleanedText.ReplaceInline(TEXT("\r"), TEXT(""));
	CleanedText.ReplaceInline(TEXT("\n"), TEXT(""));
	return CleanedText;
}

void UCommonFunctionLibrary::SetCollisionDebugVisible(UPrimitiveComponent* Target, bool bVisible)
{
	if (!Target)
		return;

	Target->SetHiddenInGame(!bVisible);
	Target->SetVisibility(bVisible);
}

bool UCommonFunctionLibrary::IsValidKoreanWord(const FString& Word)
{
	if (Word.IsEmpty())
		return false;

	// 한글 유니코드 범위 체크
	// - 완성형 한글: AC00-D7A3 (가-힣)
	// - 한글 자음/모음: 1100-11FF, 3130-318F
	for (const TCHAR& Char : Word)
	{
		// 완성형 한글 (가-힣)
		if (Char >= 0xAC00 && Char <= 0xD7A3)
			continue;
		
		// 한글 자음/모음
		if ((Char >= 0x1100 && Char <= 0x11FF) || (Char >= 0x3130 && Char <= 0x318F))
			continue;

		// 공백 허용
		if (Char == TEXT(' '))
			continue;

		// 한글이 아닌 문자 발견
		return false;
	}

	return true;
}
