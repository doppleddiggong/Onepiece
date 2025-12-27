     🚀 UPopup_DailyStudy 구현 명세서

     📋 프로젝트 정보

     - 프로젝트명: Onepiece (KLingo)                                                                                                               
     - 엔진 버전: Unreal Engine 5.4                                                                                                                
     - 모듈명: ONEPIECE_API                                                                                                                        
     - 로그 카테고리: LogOnepiece                                                                                                                  
     - 구현 목표: 데일리 단어 반복 학습 팝업 (Listen → Speak → Judge 루프)                                                                         

     🎯 구현 목표

     Daily Study는 이미지 기반 단어를 음성으로 듣고(Listen), 말하고(Speak), 판정받는(Judge) 데일리 반복 학습 팝업입니다.                          

     핵심 특징

     - 한 세션당 10개의 랜덤 단어 학습 (조정 가능)                                                                                                 
     - 순차적 진행 (FSM 불필요, 사용자 요청 사항)                                                                                                  
     - 문제 제시 → 음성 듣기 → 발화 → 판정 → 다음 문제                                                                                             
     - 10개 완료 후 종합 점수 Summary 표시                                                                                                         

     기존 시스템과의 차이점

     | 항목       | UPopup_InterviewHello | UPopup_DailyStudy      |                                                                              
     |------------|-----------------------|-------------------------|                                                                              
     | 표시 방식  | 텍스트 질문 + 입력란  | 이미지 + 음성           |                                                                              
     | 입력 방식  | 텍스트 입력           | 음성 녹음               |                                                                              
     | 네비게이션 | Prev/Next 버튼        | 순차 진행만 가능        |                                                                              
     | 진행 방식  | 자유 이동 가능        | 순차적 (돌아갈 수 없음) |                                                                              
     | 최종 결과  | 서버 제출             | 점수 Summary 표시       |                                                                              


     ---                                                                                                                                           
     📌 1. 클래스 구조

     상속 관계

     UPopup_DailyStudy : public UBasePopup                                                                                                        
     - UBasePopup은 모든 팝업의 기본 클래스                                                                                                        
     - OpenAnimation() 기능 제공                                                                                                                   
     - Copyright 헤더 포함 필수                                                                                                                    

     파일 위치

     - Header: Source/Onepiece/MessageBox/Public/UPopup_DailyStudy.h                                                                              
     - Source: Source/Onepiece/MessageBox/Private/UPopup_DailyStudy.cpp                                                                           

     ---                                                                                                                                           
     📌 2. UI 위젯 구조 (BindWidget)

     질문 표시 영역

| 위젯 이름           | 타입         | 설명                    |
|---------------------|--------------|-------------------------|
| Txt_Title           | UTextBlock   | 타이틀 ("Daily Study") |
| Txt_BestScore       | UTextBlock   | 일일 최고 득점 표시 ("Best: 85/100") |
| Img_WordDisplay   | UImage       | 단어 이미지 표시        |
| Txt_QuestionCounter | UTextBlock   | 진행 상황 ("1/10")      |
| ProgressBar_Overall | UProgressBar | 전체 진행률 표시        |
| ImageProgressBar_Overall | UProgressBar | 전체 진행률 표시        |                                                                              

     상호작용 버튼

     | 위젯 이름  | 타입                      | 설명                          |                                                                    
     |------------|---------------------------|-------------------------------|                                                                    
     | Btn_Skip   | UButton                   | 현재 문제 건너뛰기 (선택사항) |                                                                    
     | Btn_Close  | UButton or UTextureButton | 팝업 닫기 (우측 상단 ❌)      |                                                                     

     녹음 상태 표시

     | 위젯 이름             | 타입            | 설명                     |                                                                        
     |-----------------------|-----------------|--------------------------|                                                                        
     | Widget_VoiceRecording | UVoiceRecording | 녹음 시각화 위젯         |                                                                        
     | Txt_RecordingStatus   | UTextBlock      | "Recording..." 상태 표시 |                                                                        

     **결과 표시**

결과 표시는 별도의 팝업 **UPopup_DailyResult**에서 처리됩니다.
모든 문제 완료 후 DailyStudy 팝업이 닫히고 DailyResult 팝업이 자동으로 열립니다.                                                          

     참고: 위젯 이름은 UMG 블루프린트의 실제 이름과 정확히 일치해야 합니다.                                                                        

     ---                                                                                                                                           
     📌 3. 데이터 구조

     FDailyStudyWordItem (로컬 단어 데이터)

     USTRUCT(BlueprintType)                                                                                                                        
     struct FDailyStudyWordItem                                                                                                                   
     {                                                                                                                                             
         GENERATED_BODY()                                                                                                                          

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         EWordType WordType;  // Animal, Color, Region, Food                                                                                       

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         int32 WordCode;      // DataTable 인덱스                                                                                                  

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         FString Eng;         // 영어 단어 (예: "Lion")                                                                                            

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         FString Kor;         // 한국어 번역 (예: "사자")                                                                                          

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         FString Pronunciation;  // 발음 기호 (IPA)                                                                                                

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         TSoftObjectPtr<UTexture2D> Texture;  // 단어 이미지                                                                                       
     };                                                                                                                                            

     데이터 로딩 방식 (사용자 요구사항)

     "void ULevelWordItem::InitInfo( EWordType WordType, int32 WordCode ) 와 같이 WordType,와 WordCode의 랜덤으로 구현."

     // UGameDataManager를 통해 데이터 로드                                                                                                        
     FDailyStudyWordItem WordItem;                                                                                                                

     if (WordType == EWordType::Animal || WordType == EWordType::Food)                                                                             
     {                                                                                                                                             
         // Read 계열 데이터 사용                                                                                                                  
         FReadData ReadData;                                                                                                                       
         UGameDataManager::Get(GetWorld())->GetReadData(WordCode, ReadData);                                                                       

         WordItem.Eng = ReadData.Eng;                                                                                                              
         WordItem.Kor = ReadData.Kor;                                                                                                              
         WordItem.Pronunciation = ReadData.Pronunciation;                                                                                          
         WordItem.Texture = ReadData.Texture;                                                                                                      
     }                                                                                                                                             
     else if (WordType == EWordType::Color)                                                                                                        
     {                                                                                                                                             
         // Color 데이터 사용                                                                                                                      
         FColorData ColorData;                                                                                                                     
         UGameDataManager::Get(GetWorld())->GetColorData(WordCode, ColorData);                                                                     

         WordItem.Eng = ColorData.Eng;                                                                                                             
         WordItem.Kor = ColorData.Kor;                                                                                                             
         WordItem.Pronunciation = ColorData.Pronunciation;                                                                                         
         // Color는 이미지 대신 색상 표시                                                                                                          
     }                                                                                                                                             
     else if (WordType == EWordType::Region)                                                                                                       
     {                                                                                                                                             
         // Listen 데이터 사용                                                                                                                     
         FListenData ListenData;                                                                                                                   
         UGameDataManager::Get(GetWorld())->GetListenData(WordCode, ListenData);                                                                   

         WordItem.Eng = ListenData.Eng;                                                                                                            
         WordItem.Kor = ListenData.Kor;                                                                                                            
         WordItem.Pronunciation = ListenData.Pronunciation;                                                                                        
         WordItem.Texture = ListenData.Texture;                                                                                                    
     }                                                                                                                                             

     FDailyStudyAnswer (문제별 답변 저장)

     USTRUCT(BlueprintType)                                                                                                                        
     struct FDailyStudyAnswer                                                                                                                     
     {                                                                                                                                             
         GENERATED_BODY()                                                                                                                          

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         int32 QuestionIndex = 0;  // 0-based (0~9)                                                                                                

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         FString ExpectedAnswer;   // 정답 단어 (영어)                                                                                             

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         FString UserAudioPath;    // 녹음 파일 경로                                                                                               

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         FResponseSpeakingJudes JudgeResult;  // API 응답 (점수)                                                                                   

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         bool bCompleted = false;  // 완료 여부                                                                                                    

         UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")                                                                                   
         bool bSkipped = false;    // 건너뛰기 여부                                                                                                
     };                                                                                                                                            

     FResponseSpeakingJudes (API 응답, 기존 구조체)

     // NetworkData.h에 이미 정의됨                                                                                                                
     USTRUCT(BlueprintType)                                                                                                                        
     struct FResponseSpeakingJudes                                                                                                                 
     {                                                                                                                                             
         GENERATED_BODY()                                                                                                                          

         UPROPERTY(BlueprintReadWrite, Category = "Speaking")                                                                                      
         int grammar_score;         // 문법 점수                                                                                                   

         UPROPERTY(BlueprintReadWrite, Category = "Speaking")                                                                                      
         int context_score;         // 맥락 점수                                                                                                   

         UPROPERTY(BlueprintReadWrite, Category = "Speaking")                                                                                      
         int final_overall_score;   // 최종 종합 점수                                                                                              

         UPROPERTY(BlueprintReadWrite, Category = "Speaking")                                                                                      
         FString final_feedback;    // 피드백 메시지                                                                                               

         void SetFromHttpResponse(const TSharedPtr<class IHttpResponse>& Response);                                                                
         void PrintData() const;                                                                                                                   
     };                                                                                                                                            

     ---                                                                                                                                           
     📌 4. 플로우 설계 (순차적 진행)

     사용자 요구사항 확인

     "상태기계는 사실 필요없음. 순차적으로 동작함. 팝업이 열리고 문제가 제시됨. 이때 문제 갯수는 10개씩. (가변으로 수정을 할수 있게), 10개         
     문제를 모두 정의하면 speakJudes를 통해서 얻을 점수를 Summary해서 유저에게 점수를 알려줌."

     "전체 그룹 랜덤으로 10개씩.(이것도 내가 나중에 조정이 가능하게 const나 static 데이터로 빼놓기)"

     플로우 단계

     Stage 1: 초기화 (InitPopup)

1. 팝업 오픈 (UPopupManager::ShowPopup(EPopupType::DailyStudy))
2. InitPopup() 호출
3. 일일 최고 득점 로드 및 표시:
```cpp
void UPopup_DailyStudy::LoadAndDisplayBestScore()
{
    int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
    FString Today = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
    FString BestScoreDate = UConfigLibrary::GetUserString(UserId, TEXT("DailyStudyBestScoreDate"), TEXT(""));
    
    int32 BestScore = 0;
    if (BestScoreDate == Today)
    {
        // 오늘의 최고 점수 로드
        BestScore = UConfigLibrary::GetUserInt(UserId, TEXT("DailyStudyBestScore"), 0);
    }
    
    // UI 업데이트
    if (Txt_BestScore)
    {
        if (BestScore > 0)
        {
            Txt_BestScore->SetText(FText::FromString(
                FString::Printf(TEXT("Best Today: %d/100"), BestScore)));
        }
        else
        {
            Txt_BestScore->SetText(FText::FromString(TEXT("Best Today: -")));
        }
    }
}
```
4. 10개 랜덤 단어 생성:                                                                                                                       
     void GenerateQuestions()                                                                                                                      
     {                                                                                                                                             
         QuestionList.Empty();                                                                                                                     

         for (int32 i = 0; i < DailyStudyConfig::QUESTIONS_PER_SESSION; ++i)                                                                      
         {                                                                                                                                         
             // 랜덤 WordType 선택                                                                                                                 
             EWordType RandomType = static_cast<EWordType>(                                                                                        
                 FMath::RandRange(0, static_cast<int32>(EWordType::Food)));                                                                        

             // WordType에 따라 DataTable 크기 확인 후 랜덤 Code 선택                                                                              
             int32 MaxCode = GetMaxWordCodeForType(RandomType);                                                                                    
             int32 RandomCode = FMath::RandRange(0, MaxCode - 1);                                                                                  

             // 데이터 로드                                                                                                                        
             FDailyStudyWordItem WordItem;                                                                                                        
             LoadWordData(RandomType, RandomCode, WordItem);                                                                                       

             QuestionList.Add(WordItem);                                                                                                           
         }                                                                                                                                         

         // AnswerList 초기화                                                                                                                      
         AnswerList.SetNum(QuestionList.Num());                                                                                                    
         for (int32 i = 0; i < AnswerList.Num(); ++i)                                                                                              
         {                                                                                                                                         
             AnswerList[i].QuestionIndex = i;                                                                                                      
             AnswerList[i].ExpectedAnswer = QuestionList[i].Eng;                                                                                   
         }                                                                                                                                         
     }                                                                                                                                             
     4. CurrentQuestionIndex = 0 설정                                                                                                              
     5. 첫 번째 질문 표시 (LoadCurrentQuestion())                                                                                                  

     Stage 2: 문제별 루프 (x10)

     각 문제마다 (index 0~9):                                                                                                                      

     1. 문제 표시

     void LoadCurrentQuestion()                                                                                                                    
     {                                                                                                                                             
         if (!QuestionList.IsValidIndex(CurrentQuestionIndex))                                                                                     
             return;                                                                                                                               

         const FDailyStudyWordItem& CurrentWord = QuestionList[CurrentQuestionIndex];                                                             

         // 이미지 로드                                                                                                                            
         UTexture2D* LoadedTexture = nullptr;                                                                                                      
         if (CurrentWord.Texture.IsValid())                                                                                                        
             LoadedTexture = CurrentWord.Texture.Get();                                                                                            
         else                                                                                                                                      
             LoadedTexture = CurrentWord.Texture.LoadSynchronous();                                                                                

         if (Image_WordDisplay)                                                                                                                    
         {                                                                                                                                         
             Image_WordDisplay->SetBrushFromTexture(LoadedTexture, true);                                                                          
         }                                                                                                                                         

         // 진행 상황 업데이트                                                                                                                     
         if (Txt_QuestionCounter)                                                                                                                  
         {                                                                                                                                         
             Txt_QuestionCounter->SetText(FText::FromString(                                                                                       
                 FString::Printf(TEXT("%d/%d"),                                                                                                    
                     CurrentQuestionIndex + 1,                                                                                                     
                     QuestionList.Num())));                                                                                                        
         }                                                                                                                                         

         // Progress Bar 업데이트                                                                                                                  
         RefreshProgressBar();                                                                                                                     

         // 버튼 초기화                                                                                                                            
         Btn_Speak->SetIsEnabled(true);                                                                                                            
         Btn_Listen->SetIsEnabled(true);                                                                                                           
     }                                                                                                                                             

     2. Listen 단계 (선택사항, 반복 가능)

     사용자가 Btn_Listen 클릭 시:                                                                                                                  

     void OnClickListen()                                                                                                                          
     {                                                                                                                                             
         const FDailyStudyWordItem& CurrentWord = QuestionList[CurrentQuestionIndex];                                                             

         if (UKLingoNetworkSystem* NetSys = UKLingoNetworkSystem::Get(GetWorld()))                                                                 
         {                                                                                                                                         
             NetSys->RequestListenAudio(                                                                                                           
                 CurrentWord.Eng,                                                                                                                  
                 FResponseListenAudioDelegate::CreateUObject(                                                                                      
                     this, &UPopup_DailyStudy::OnResponseListenAudio)                                                                             
             );                                                                                                                                    
         }                                                                                                                                         
     }                                                                                                                                             

     void OnResponseListenAudio(FResponseListenAudio& Response, bool bSuccess)                                                                     
     {                                                                                                                                             
         if (bSuccess && Response.audio_base64.Num() > 0)                                                                                          
         {                                                                                                                                         
             // UGameSoundManager를 통해 재생                                                                                                      
             if (UGameSoundManager* SoundMgr = UGameSoundManager::Get(GetWorld()))                                                                 
             {                                                                                                                                     
                 // Base64 → USoundWave 변환                                                                                                       
                 USoundWave* SoundWave = UVoiceFunctionLibrary::CreateSoundWaveFromWavData(                                                        
                     Response.audio_base64);                                                                                                       

                 if (SoundWave)                                                                                                                    
                 {                                                                                                                                 
                     SoundMgr->PlayConversationVoice(SoundWave);                                                                                   
                 }                                                                                                                                 
             }                                                                                                                                     
         }                                                                                                                                         
     }                                                                                                                                             

     3. Speak 단계 (필수)

     사용자가 Btn_Speak 클릭 시:                                                                                                                   

     void OnClickSpeak()                                                                                                                           
     {                                                                                                                                             
         if (bIsRecording)                                                                                                                         
         {                                                                                                                                         
             // 녹음 중지                                                                                                                          
             StopSpeakRecording();                                                                                                                 
         }                                                                                                                                         
         else                                                                                                                                      
         {                                                                                                                                         
             // 녹음 시작                                                                                                                          
             StartSpeakRecording();                                                                                                                
         }                                                                                                                                         
     }                                                                                                                                             

     void StartSpeakRecording()                                                                                                                    
     {                                                                                                                                             
         if (UVoiceConversationSystem* VoiceSys = UVoiceConversationSystem::Get(GetWorld()))                                                       
         {                                                                                                                                         
             VoiceSys->StartRecording();                                                                                                           
             bIsRecording = true;                                                                                                                  

             // UI 업데이트                                                                                                                        
             if (Txt_RecordingStatus)                                                                                                              
             {                                                                                                                                     
                 Txt_RecordingStatus->SetText(FText::FromString(TEXT("Recording...")));                                                            
             }                                                                                                                                     

             if (Widget_VoiceRecording)                                                                                                            
             {                                                                                                                                     
                 Widget_VoiceRecording->SetVisibility(ESlateVisibility::Visible);                                                                  
             }                                                                                                                                     

             // 자동 중지 타이머 (5초 후)                                                                                                          
             GetWorld()->GetTimerManager().SetTimer(                                                                                               
                 RecordingTimerHandle,                                                                                                             
                 this,                                                                                                                             
                 &UPopup_DailyStudy::StopSpeakRecording,                                                                                          
                 DailyStudyConfig::RECORDING_MAX_DURATION,                                                                                        
                 false                                                                                                                             
             );                                                                                                                                    
         }                                                                                                                                         
     }                                                                                                                                             

     void StopSpeakRecording()                                                                                                                     
     {                                                                                                                                             
         if (UVoiceConversationSystem* VoiceSys = UVoiceConversationSystem::Get(GetWorld()))                                                       
         {                                                                                                                                         
             VoiceSys->StopRecording();                                                                                                            
             bIsRecording = false;                                                                                                                 

             // 타이머 해제                                                                                                                        
             GetWorld()->GetTimerManager().ClearTimer(RecordingTimerHandle);                                                                       

             // UI 업데이트                                                                                                                        
             if (Widget_VoiceRecording)                                                                                                            
             {                                                                                                                                     
                 Widget_VoiceRecording->SetVisibility(ESlateVisibility::Hidden);                                                                   
             }                                                                                                                                     

             // 녹음 파일 저장                                                                                                                     
             int32 UserId = ULingoGameHelper::GetUserId(GetWorld());                                                                               
             FString SavePath = FPaths::Combine(                                                                                                   
                 FPaths::ProjectSavedDir(),                                                                                                        
                 TEXT("DailyStudyRecordings"),                                                                                                    
                 FString::Printf(TEXT("User_%d_Q%d_%s.wav"),                                                                                       
                     UserId, CurrentQuestionIndex, *FDateTime::Now().ToString())                                                                   
             );                                                                                                                                    

             // TODO: VoiceConversationSystem에서 PCM 데이터 가져와서 WAV로 저장                                                                   
             // AnswerList[CurrentQuestionIndex].UserAudioPath = SavePath;                                                                         

             // Judge API 호출                                                                                                                     
             SubmitSpeakJudge();                                                                                                                   
         }                                                                                                                                         
     }                                                                                                                                             

     4. Judge 단계 (자동)

     녹음 완료 후 자동으로 API 호출:                                                                                                               

     void SubmitSpeakJudge()                                                                                                                       
     {                                                                                                                                             
         const FString& AudioPath = AnswerList[CurrentQuestionIndex].UserAudioPath;                                                                
         const FString& ExpectedAnswer = AnswerList[CurrentQuestionIndex].ExpectedAnswer;                                                          

         if (UKLingoNetworkSystem* NetSys = UKLingoNetworkSystem::Get(GetWorld()))                                                                 
         {                                                                                                                                         
             NetSys->RequestSpeakingJudges(                                                                                                        
                 ExpectedAnswer,                                                                                                                   
                 AudioPath,                                                                                                                        
                 FResponseSpeakingJudesDelegate::CreateUObject(                                                                                    
                     this, &UPopup_DailyStudy::OnResponseSpeakingJudges)                                                                          
             );                                                                                                                                    
         }                                                                                                                                         
     }                                                                                                                                             

     void OnResponseSpeakingJudges(FResponseSpeakingJudes& Response, bool bSuccess)                                                                
     {                                                                                                                                             
         if (bSuccess)                                                                                                                             
         {                                                                                                                                         
             // 결과 저장                                                                                                                          
             AnswerList[CurrentQuestionIndex].JudgeResult = Response;                                                                              
             AnswerList[CurrentQuestionIndex].bCompleted = true;                                                                                   

             // 간단한 피드백 표시 (선택사항)                                                                                                      
             FString FeedbackMsg = FString::Printf(                                                                                                
                 TEXT("Score: %d/100"),                                                                                                            
                 Response.final_overall_score);                                                                                                    

             if (UDialogManager* DialogMgr = UDialogManager::Get(GetWorld()))                                                                      
             {                                                                                                                                     
                 DialogMgr->ShowToast(FeedbackMsg);                                                                                                
             }                                                                                                                                     

             // 다음 문제로 이동 (1초 후)                                                                                                          
             GetWorld()->GetTimerManager().SetTimer(                                                                                               
                 NextQuestionTimerHandle,                                                                                                          
                 this,                                                                                                                             
                 &UPopup_DailyStudy::MoveToNextQuestion,                                                                                          
                 1.0f,                                                                                                                             
                 false                                                                                                                             
             );                                                                                                                                    
         }                                                                                                                                         
         else                                                                                                                                      
         {                                                                                                                                         
             // API 실패 처리                                                                                                                      
             if (UDialogManager* DialogMgr = UDialogManager::Get(GetWorld()))                                                                      
             {                                                                                                                                     
                 DialogMgr->ShowToast(TEXT("Failed to judge. Please try again."));                                                                 
             }                                                                                                                                     

             // 재시도 가능하도록 버튼 활성화                                                                                                      
             Btn_Speak->SetIsEnabled(true);                                                                                                        
         }                                                                                                                                         
     }                                                                                                                                             

     5. 다음 문제로 이동

     void MoveToNextQuestion()
{
    CurrentQuestionIndex++;

    if (CurrentQuestionIndex < QuestionList.Num())
    {
        // 다음 문제 로드
        LoadCurrentQuestion();
    }
    else
    {
        // 모든 문제 완료 → 결과 팝업 표시
        ShowResultPopup();
    }
}                                                                                                                                             

     Stage 3: 결과 팝업 표시

모든 10문제 완료 후:

```cpp
void ShowResultPopup()
{
    PRINTLOG(TEXT("[DailyStudy] Opening result popup"));

    // DailyResult 팝업 오픈
    UPopupManager* PopupMgr = UPopupManager::Get(GetWorld());
    if (!PopupMgr)
    {
        return;
    }

    // 현재 팝업(DailyStudy) 닫기
    PopupMgr->HideCurrentPopup();

    // DailyResult 팝업 열기
    UBasePopup* ResultPopup = PopupMgr->ShowPopup(EPopupType::DailyResult);
    if (UPopup_DailyResult* DailyResultPopup = Cast<UPopup_DailyResult>(ResultPopup))
    {
        // 답변 목록 전달하여 초기화
        DailyResultPopup->InitPopup(AnswerList);
    }
}
```

**UPopup_DailyResult**에서 처리되는 내용:
- 점수 집계 (평균 문법, 맥락, 최종 점수)
- 피드백 메시지 생성
- UI 업데이트
- 일일 최고 득점 저장
- Config에 진행 상황 저장                                                                                                                                             

     ---
📌 4.1 PlayerActor VoiceConversationSystem 연동

**핵심 요구사항**

PopupDailyStudy가 열려있을 때, PlayerActor의 VoiceConversationSystem을 통해 StartRecording이 호출되면:
1. 해당 녹음은 PopupDailyStudy로 라우팅되어야 함
2. PopupDailyStudy에서 SpeakJudges 판정을 받아야 함
3. PopupDailyStudy의 Progress가 정상적으로 진행되어야 함

**구현 방법: PopupManager를 통한 라우팅**

PlayerActor → PopupManager → DailyStudy 팝업 순서로 데이터가 전달됩니다.

**1. PlayerActor에서 녹음 완료 처리**

```cpp
// PlayerActor.cpp (또는 PlayerController.cpp)
void APlayerActor::OnRecordingComplete(const TArray<uint8>& AudioData, bool bSuccess)
{
    // PopupManager로 녹음 데이터 라우팅
    if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
    {
        PopupMgr->RouteRecordingData(AudioData, bSuccess);
    }
}
```

**2. PopupManager의 라우팅 로직**

```cpp
// UPopupManager.cpp
void UPopupManager::RouteRecordingData(const TArray<uint8>& AudioData, bool bSuccess)
{
    // 현재 활성 팝업 타입 확인
    EPopupType CurrentType;
    if (!GetCurrentPopupType(CurrentType))
    {
        return; // 활성 팝업이 없으면 무시
    }

    // DailyStudy 팝업인 경우에만 처리
    if (CurrentType == EPopupType::DailyStudy)
    {
        UUserWidget* CurrentWidget = GetCurrentPopupWidget();
        if (UPopup_DailyStudy* DailyStudyPopup = Cast<UPopup_DailyStudy>(CurrentWidget))
        {
            DailyStudyPopup->HandleRecordingComplete(AudioData, bSuccess);
        }
    }
    // 다른 팝업이 열려있으면 무시
}
```

**3. UPopup_DailyStudy의 처리**

```cpp
// UPopup_DailyStudy.cpp
void UPopup_DailyStudy::HandleRecordingComplete(const TArray<uint8>& AudioData, bool bSuccess)
{
    if (!bSuccess || AudioData.Num() == 0)
    {
        return;
    }
    
    // 녹음 파일 저장
    int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
    FString SavePath = FPaths::Combine(
        FPaths::ProjectSavedDir(),
        TEXT("DailyStudyRecordings"),
        FString::Printf(TEXT("User_%d_Q%d_%s.wav"),
            UserId, CurrentQuestionIndex, *FDateTime::Now().ToString())
    );
    
    // WAV 파일로 저장
    FFileHelper::SaveArrayToFile(AudioData, *SavePath);
    AnswerList[CurrentQuestionIndex].UserAudioPath = SavePath;
    
    // Judge API 호출
    SubmitSpeakJudge();
}
```

**장점**

- PlayerActor가 팝업에 직접 의존하지 않음
- PopupManager가 중앙에서 라우팅 제어
- 다른 팝업 추가 시 확장 용이
- 명확한 의존성 관리

---
📌 4.2 UPopup_DailyResult (결과 팝업)

**개요**

Daily Study 세션 완료 후 결과를 표시하는 별도의 팝업입니다.
UPopup_DailyStudy에서 모든 문제 완료 시 자동으로 열립니다.

**파일 위치**

- Header: Source/Onepiece/MessageBox/Public/UPopup_DailyResult.h
- Source: Source/Onepiece/MessageBox/Private/UPopup_DailyResult.cpp

**UI 위젯 구조**

| 위젯 이름 | 타입 | 설명 |
|-----------|------|------|
| Txt_FinalScore | UTextBlock | 최종 점수 ("Your Score: 85/100") |
| Txt_Feedback | UTextBlock | 종합 피드백 메시지 |
| Txt_CompletedCount | UTextBlock | 완료/총 개수 ("Completed: 9/10") |
| Btn_Confirm | UButton | 확인 버튼 (팝업 닫기) |

**주요 기능**

1. **결과 집계** (CalculateResults)
   - 평균 문법 점수
   - 평균 맥락 점수
   - 평균 최종 점수
   - 완료한 문제 개수
   - 건너뛴 문제 개수

2. **피드백 메시지 생성** (GenerateFeedbackMessage)
   - 80점 이상: "Excellent! Great pronunciation!"
   - 60~79점: "Good job! Keep practicing!"
   - 60점 미만: "Keep trying! Practice makes perfect!"

3. **데이터 저장** (SaveProgress)
   - 플레이 횟수 증가
   - 마지막 점수 저장
   - **일일 최고 득점 업데이트**
   - 마지막 플레이 날짜 저장

**초기화 메서드**

```cpp
void UPopup_DailyResult::InitPopup(const TArray<FDailyStudyAnswer>& AnswerList)
{
    // 결과 집계
    ResultSummary = CalculateResults(AnswerList);
    
    // UI 업데이트
    UpdateUI(ResultSummary);
    
    // 저장
    SaveProgress(ResultSummary.AvgFinalScore);
    
    // 오픈 애니메이션
    OpenAnimation();
}
```

---
📌 5. 설정 상수 (DailyStudyConfig)

     네임스페이스 또는 Static 변수

     사용자 요구사항: "이것도 내가 나중에 조정이 가능하게 const나 static 데이터로 빼놓기"

     // In UPopup_DailyStudy.h or .cpp                                                                                                            
     namespace DailyStudyConfig                                                                                                                   
     {                                                                                                                                             
         /** 한 세션당 문제 개수 (조정 가능) */                                                                                                    
         static constexpr int32 QUESTIONS_PER_SESSION = 10;                                                                                        

         /** 녹음 최대 지속 시간 (초) */                                                                                                           
         static constexpr float RECORDING_MAX_DURATION = 5.0f;                                                                                     

         /** Listen 오디오 볼륨 */                                                                                                                 
         static constexpr float LISTEN_AUDIO_VOLUME = 1.0f;                                                                                        

         /** 다음 문제로 넘어가기 전 대기 시간 (초) */                                                                                             
         static constexpr float NEXT_QUESTION_DELAY = 1.0f;                                                                                        
     }                                                                                                                                             

     대안: Config 파일 사용

     나중에 설정 파일로 빼려면:                                                                                                                    

     // DefaultGame.ini 또는 커스텀 Config                                                                                                         
     [/Script/Onepiece.DailyStudySettings]                                                                                                        
     QuestionsPerSession=10                                                                                                                        
     RecordingMaxDuration=5.0                                                                                                                      

     ---                                                                                                                                           
     📌 6. 필수 메서드 구조

     6.1 Public 메서드

     /** 팝업 초기화 */                                                                                                                            
     UFUNCTION(BlueprintCallable, Category = "Popup")                                                                                              
     void InitPopup();                                                                                                                             

     6.2 Protected 메서드

     protected:                                                                                                                                    
         virtual void NativeConstruct() override;                                                                                                  

     6.3 Private 메서드

     초기화 및 데이터 로드

     private:                                                                                                                                      
         /** 10개 랜덤 단어 생성 */                                                                                                                
         void GenerateQuestions();                                                                                                                 

         /** WordType에 맞는 데이터 로드 */                                                                                                        
         void LoadWordData(EWordType Type, int32 Code, FDailyStudyWordItem& OutItem);                                                             

         /** WordType별 최대 Code 개수 반환 */                                                                                                     
         int32 GetMaxWordCodeForType(EWordType Type) const;                                                                                        

     UI 업데이트

    /** 일일 최고 득점 로드 및 표시 */
    void LoadAndDisplayBestScore();

    /** 현재 질문 UI 업데이트 */
    void LoadCurrentQuestion();

    /** Progress Bar 업데이트 */
    void RefreshProgressBar();

    /** 결과 팝업 표시 (DailyResult 팝업 오픈) */
    void ShowResultPopup();                                                                                                                   

     문제 네비게이션

         /** 다음 질문으로 이동 */                                                                                                                 
         void MoveToNextQuestion();                                                                                                                

     오디오 처리

         /** Listen 오디오 재생 */                                                                                                                 
         void PlayListenAudio();                                                                                                                   

         /** 녹음 시작 */                                                                                                                          
         void StartSpeakRecording();                                                                                                               

         /** 녹음 중지 */                                                                                                                          
         void StopSpeakRecording();                                                                                                                

         /** Judge API 제출 */                                                                                                                     
         void SubmitSpeakJudge();                                                                                                                  

     버튼 이벤트

         /** Listen 버튼 클릭 */                                                                                                                   
         UFUNCTION()                                                                                                                               
         void OnClickListen();                                                                                                                     

         /** Speak 버튼 클릭 */                                                                                                                    
         UFUNCTION()                                                                                                                               
         void OnClickSpeak();                                                                                                                      

         /** Skip 버튼 클릭 */                                                                                                                     
         UFUNCTION()                                                                                                                               
         void OnClickSkip();                                                                                                                       

         /** 닫기 버튼 클릭 */                                                                                                                     
         UFUNCTION()                                                                                                                               
         void OnClickClose();                                                                                                                      

                                                                                                                             

     API 콜백

         /** Listen Audio API 응답 */                                                                                                              
         UFUNCTION()                                                                                                                               
         void OnResponseListenAudio(FResponseListenAudio& Response, bool bSuccess);                                                                

         /** Speaking Judges API 응답 */                                                                                                           
         UFUNCTION()                                                                                                                               
         void OnResponseSpeakingJudges(FResponseSpeakingJudes& Response, bool bSuccess);                                                           

                                                                                                           

     6.4 중요 원칙

     - Null 체크 필수: 모든 위젯 포인터 사용 전 if (Widget) 체크                                                                                   
     - 범위 체크: 배열 접근 시 IsValidIndex() 사용                                                                                                 
     - 예외 없이 완전 동작: 어떤 상황에서도 크래시 없이 안전하게 동작                                                                              

     ---                                                                                                                                           
     📌 7. 프로젝트 시스템 통합

     7.1 필수 Include

     // Header                                                                                                                                     
     #include "UBasePopup.h"                                                                                                                       
     #include "EWordType.h"                                                                                                                        
     #include "NetworkData.h"                                                                                                                      

     // Source                                                                                                                                     
     #include "UPopupManager.h"                                                                                                                    
     #include "UKLingoNetworkSystem.h"                                                                                                             
     #include "ULingoGameHelper.h"                                                                                                                 
     #include "UGameDataManager.h"                                                                                                                 
     #include "UGameSoundManager.h"                                                                                                                
     #include "UVoiceConversationSystem.h"                                                                                                         
     #include "UVoiceFunctionLibrary.h"                                                                                                            
     #include "UDialogManager.h"                                                                                                                   
     #include "UConfigLibrary.h"                                                                                                                   
     #include "GameLogging.h"                                                                                                                      
     #include "Components/Image.h"                                                                                                                 
     #include "Components/TextBlock.h"                                                                                                             
     #include "Components/Button.h"                                                                                                                
     #include "Components/ProgressBar.h"                                                                                                           
     #include "UVoiceRecording.h"                                                                                                                  
     // ... 기타 사용하는 위젯 컴포넌트                                                                                                            

     7.2 Manager 클래스 사용

     // 팝업 닫기                                                                                                                                  
     UPopupManager::Get(GetWorld())->HideCurrentPopup();                                                                                           

     // 네트워크 요청 (Listen)                                                                                                                     
     UKLingoNetworkSystem::Get(GetWorld())->RequestListenAudio(...);                                                                               

     // 네트워크 요청 (Judge)                                                                                                                      
     UKLingoNetworkSystem::Get(GetWorld())->RequestSpeakingJudges(...);                                                                            

     // Toast 메시지                                                                                                                               
     UDialogManager::Get(GetWorld())->ShowToast(Message);                                                                                          

     // 데이터 로드                                                                                                                                
     UGameDataManager::Get(GetWorld())->GetReadData(Code, OutData);                                                                                

     // 오디오 재생                                                                                                                                
     UGameSoundManager::Get(GetWorld())->PlayConversationVoice(SoundWave);                                                                         

     // 녹음 시작/중지                                                                                                                             
     UVoiceConversationSystem::Get(GetWorld())->StartRecording();                                                                                  
     UVoiceConversationSystem::Get(GetWorld())->StopRecording();                                                                                   

     // UserId 가져오기                                                                                                                            
     int32 UserId = ULingoGameHelper::GetUserId(GetWorld());                                                                                       

     // Config 저장                                                                                                                                
     UConfigLibrary::SetUserInt(UserId, Key, Value);                                                                                               

     7.3 로그

     #include "GameLogging.h"                                                                                                                      

     PRINTLOG(TEXT("[DailyStudy] Question %d loaded: %s"),                                                                                        
         CurrentQuestionIndex, *CurrentWord.Eng);                                                                                                  

     ---                                                                                                                                           
     📌 8. 데이터 저장 (Config 기반)

     8.1 저장 항목

- 총 플레이 횟수
- 마지막 플레이 날짜
- 마지막 점수
- **일일 최고 득점 (DailyStudyBestScore)** - 오늘 날짜의 최고 점수
- 평균 점수 (선택사항)                                                                                                                        

     8.2 구현 예시

     void UPopup_DailyStudy::SaveProgress(int32 FinalScore)
{
    int32 UserId = ULingoGameHelper::GetUserId(GetWorld());

    // 플레이 횟수 증가
    int32 CurrentCount = UConfigLibrary::GetUserInt(UserId, TEXT("DailyStudyCount"), 0);
    UConfigLibrary::SetUserInt(UserId, TEXT("DailyStudyCount"), CurrentCount + 1);

    // 마지막 점수 저장
    UConfigLibrary::SetUserInt(UserId, TEXT("DailyStudyLastScore"), FinalScore);

    // 일일 최고 득점 업데이트
    FString Today = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
    FString LastBestDate = UConfigLibrary::GetUserString(UserId, TEXT("DailyStudyBestScoreDate"), TEXT(""));
    
    if (LastBestDate != Today)
    {
        // 새로운 날, 오늘의 최고 점수로 설정
        UConfigLibrary::SetUserInt(UserId, TEXT("DailyStudyBestScore"), FinalScore);
        UConfigLibrary::SetUserString(UserId, TEXT("DailyStudyBestScoreDate"), Today);
    }
    else
    {
        // 같은 날, 기존 최고 점수와 비교
        int32 CurrentBest = UConfigLibrary::GetUserInt(UserId, TEXT("DailyStudyBestScore"), 0);
        if (FinalScore > CurrentBest)
        {
            UConfigLibrary::SetUserInt(UserId, TEXT("DailyStudyBestScore"), FinalScore);
        }
    }

    // 마지막 플레이 날짜 저장
    UConfigLibrary::SetUserString(UserId, TEXT("DailyStudyLastDate"), Today);

    PRINTLOG(TEXT("[DailyStudy] Progress saved for User %d, Score: %d"), UserId, FinalScore);
}                                                                                                                                             

     8.3 데이터 읽기 (진입 조건 체크 등)

     bool ShouldShowDailyStudyToday(const UObject* WorldContext)                                                                                  
     {                                                                                                                                             
         int32 UserId = ULingoGameHelper::GetUserId(WorldContext);                                                                                 
         FString LastDate = UConfigLibrary::GetUserString(UserId, TEXT("DailyStudyLastDate"), TEXT(""));                                          

         FString Today = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));                                                                              

         // 오늘 이미 플레이했으면 건너뜀                                                                                                          
         if (LastDate == Today)                                                                                                                    
         {                                                                                                                                         
             return false;                                                                                                                         
         }                                                                                                                                         

         return true;                                                                                                                              
     }                                                                                                                                             

     ---                                                                                                                                           
     📌 9. EPopupType 등록

     9.1 EPopupType.h에 추가

     enum class EPopupType : uint8                                                                                                                 
     {                                                                                                                                             
         // ... 기존 타입들                                                                                                                        
         MsgBox,                                                                                                                                   
         InterviewHello,                                                                                                                           
         AskTutorial,                                                                                                                              
         // ... 중략                                                                                                                               

         DailyStudy,  // NEW                                                                                                                      

         // ... 기타                                                                                                                               
     };                                                                                                                                            

     9.2 UPopupManager 생성자에 등록

     // UPopupManager.cpp                                                                                                                          
     UPopupManager::UPopupManager()                                                                                                                
     {                                                                                                                                             
         // ... 기존 등록                                                                                                                          

         PopupClassMap.Add(EPopupType::DailyStudy,                                                                                                
             FComponentHelper::LoadClass<UPopup_DailyStudy>(DAILYSTUDY_POPUP_PATH));                                                             
     }                                                                                                                                             

     9.3 경로 상수 정의

     // UPopupManager.h 또는 별도 Config                                                                                                           
     #define DAILYSTUDY_POPUP_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_PopupDailyStudy_C")                                                 

     ---                                                                                                                                           
     📌 10. 체크리스트 (구현 필수 기능)

     ✅ 핵심 기능

     - 10개 랜덤 단어 생성 (모든 EWordType에서 랜덤 선택)                                                                                          
     - 순차적 문제 진행 (FSM 없음)                                                                                                                 
     - Listen 버튼 (TTS 재생, 반복 가능)                                                                                                           
     - Speak 버튼 (녹음 시작/중지)                                                                                                                 
     - Skip 버튼 (선택사항)                                                                                                                        
     - 진행 상태 표시 ("1/10", ProgressBar)                                                                                                        
     - 문제별 Judge API 호출                                                                                                                       
     - 최종 Summary 패널 (종합 점수 표시)                                                                                                          

     ✅ API 통신

     - RequestListenAudio() 호출 (TTS)                                                                                                             
     - OnResponseListenAudio() 콜백 처리                                                                                                           
     - RequestSpeakingJudges() 호출 (STT + Scoring)                                                                                                
     - OnResponseSpeakingJudges() 콜백 처리                                                                                                        
     - API 실패 시 사용자 피드백                                                                                                                   

     ✅ 음성 처리

     - UVoiceConversationSystem 연동 (녹음)                                                                                                        
     - UGameSoundManager 연동 (TTS 재생)                                                                                                           
     - UVoiceRecording 위젯 표시 (녹음 시각화)                                                                                                     
     - 녹음 파일 저장 (ProjectSavedDir/DailyStudyRecordings/)                                                                                     
     - 녹음 자동 중지 (5초 타이머)                                                                                                                 

     ✅ 데이터 관리

- FDailyStudyWordItem 구조체 정의
- FDailyStudyAnswer 배열 저장 (10개)
- UGameDataManager를 통한 단어 데이터 로드
- 점수 집계 로직 (평균 계산)
- Config 저장 (플레이 횟수, 점수, 날짜, **일일 최고 득점**)
- **일일 최고 득점 로드 및 표시**
- **PlayerActor VoiceConversationSystem 연동**                                                                                                       

     ✅ UI/UX

     - 모든 위젯 BindWidget 선언                                                                                                                   
     - NativeConstruct에서 버튼 바인딩                                                                                                             
     - Null 체크 및 범위 체크                                                                                                                      
     - 결과 패널 표시/숨김 처리                                                                                                                    
     - 부드러운 화면 전환                                                                                                                          

     ✅ 설정 및 조정

     - DailyStudyConfig::QUESTIONS_PER_SESSION 상수                                                                                               
     - DailyStudyConfig::RECORDING_MAX_DURATION 상수                                                                                              
     - 기타 조정 가능한 파라미터를 const/static으로 분리                                                                                           

     ---                                                                                                                                           
     📌 11. 코드 생성 가이드

     11.1 파일 구조

     Source/Onepiece/MessageBox/                                                                                                                   
     ├── Public/                                                                                                                                   
     │   └── UPopup_DailyStudy.h
     └── Private/
         └── UPopup_DailyStudy.cpp                                                                                                                

     11.2 Copyright 헤더

     // Copyright (c) 2025 Doppleddiggong. All rights reserved.                                                                                    
     // Unauthorized copying, modification, or distribution of this file,                                                                          
     // via any medium is strictly prohibited. Proprietary and confidential.                                                                       

     11.3 클래스 선언 예시

UCLASS()
class ONEPIECE_API UPopup_DailyStudy : public UBasePopup
{
    GENERATED_BODY()

public:
    // Public 메서드
    UFUNCTION(BlueprintCallable, Category = "Popup")
    void InitPopup();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;                                                                                                  

     private:
    // Private 메서드
    void GenerateQuestions();
    void LoadCurrentQuestion();
    void RefreshProgressBar();
    void ShowResultPanel();
    void LoadAndDisplayBestScore();
    
    /** VoiceConversationSystem 녹음 완료 콜백 */
    UFUNCTION()
    void OnVoiceSystemRecordingComplete(const TArray<uint8>& AudioData, bool bSuccess);
    // ... (기타 메서드)                                                                                                                      

     protected:                                                                                                                                    
         // BindWidget 위젯들
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_Title;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_BestScore;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> Image_WordDisplay;                                                                                               

         UPROPERTY(meta = (BindWidget))                                                                                                            
         TObjectPtr<class UTextBlock> Txt_QuestionCounter;                                                                                         

         UPROPERTY(meta = (BindWidget))                                                                                                            
         TObjectPtr<class UProgressBar> ProgressBar_Overall;                                                                                       

         UPROPERTY(meta = (BindWidget))                                                                                                            
         TObjectPtr<class UButton> Btn_Listen;                                                                                                     

         UPROPERTY(meta = (BindWidget))                                                                                                            
         TObjectPtr<class UButton> Btn_Speak;                                                                                                      

         UPROPERTY(meta = (BindWidget))                                                                                                            
         TObjectPtr<class UButton> Btn_Skip;                                                                                                       

         UPROPERTY(meta = (BindWidget))                                                                                                            
         TObjectPtr<class UButton> Btn_Close;                                                                                                      

         UPROPERTY(meta = (BindWidget))                                                                                                            
         TObjectPtr<class UVoiceRecording> Widget_VoiceRecording;                                                                                  

         UPROPERTY(meta = (BindWidget))                                                                                                            
         TObjectPtr<class UTextBlock> Txt_RecordingStatus;                                                                                         

                                                                                                             

     private:                                                                                                                                      
         // 멤버 변수                                                                                                                              
         TArray<FDailyStudyWordItem> QuestionList;                                                                                                
         TArray<FDailyStudyAnswer> AnswerList;                                                                                                    
         int32 CurrentQuestionIndex = 0;                                                                                                           
         bool bIsRecording = false;                                                                                                                

         FTimerHandle RecordingTimerHandle;                                                                                                        
         FTimerHandle NextQuestionTimerHandle;                                                                                                     
     };                                                                                                                                            

     11.4 빌드 요구사항

     - UE 5.4 호환                                                                                                                                 
     - ONEPIECE_API 매크로 사용                                                                                                                    
     - Unreal C++ 코딩 컨벤션 준수                                                                                                                 
     - TObjectPtr<> 사용 (UE5 스타일)                                                                                                              
     - 실제 프로젝트에서 즉시 빌드 가능                                                                                                            

     ---                                                                                                                                           
     📌 12. 참고 자료

     기존 구현 참조

     - Source/Onepiece/MessageBox/Public/UPopup_InterviewHello.h (복잡한 팝업 예시)                                                                
     - Source/Onepiece/MessageBox/Private/UPopup_InterviewHello.cpp                                                                                
     - Source/Onepiece/MessageBox/Public/ULevelWordItem.h (단어 아이템 위젯)                                                                       
     - Source/Onepiece/MessageBox/Private/ULevelWordItem.cpp                                                                                       

     관련 시스템

     - UBasePopup: 팝업 베이스 클래스                                                                                                              
     - UPopupManager: 팝업 표시/숨김 관리                                                                                                          
     - UKLingoNetworkSystem: API 통신                                                                                                              
     - UGameDataManager: 마스터 데이터 로드                                                                                                        
     - UGameSoundManager: 오디오 재생                                                                                                              
     - UVoiceConversationSystem: 음성 녹음                                                                                                         
     - UVoiceFunctionLibrary: 오디오 유틸리티                                                                                                      
     - UConfigLibrary: 설정 저장/로드                                                                                                              
     - NetworkData.h: 데이터 구조 정의                                                                                                             

     데이터 타입

     - EWordType: Animal, Color, Region, Food                                                                                                      
     - FReadData, FListenData, FColorData: 단어 데이터 구조 (UGameDataManager)                                                                     
     - FResponseSpeakingJudes: Speaking 판정 API 응답                                                                                              

     ---                                                                                                                                           
     📝 구현 시 주의사항

     1. 위젯 이름 정확히 일치: UMG 블루프린트의 위젯 이름과 BindWidget 변수명이 정확히 일치해야 합니다.                                            
     2. 버튼 바인딩: NativeConstruct()에서 모든 버튼 이벤트를 바인딩합니다.                                                                        
     3. 중복 방지: RemoveDynamic → AddDynamic 패턴 사용.                                                                                           
     4. 인덱스 관리: CurrentQuestionIndex가 유효 범위 내에 있는지 항상 확인.                                                                       
     5. UI 동기화: 문제 이동 시마다 LoadCurrentQuestion(), RefreshProgressBar() 호출.                                                              
     6. 녹음 파일 관리: 임시 파일이 누적되지 않도록 정리 로직 고려.                                                                                
     7. API 에러 처리: 네트워크 실패 시 사용자에게 명확한 메시지 표시.                                                                             
     8. 타이머 정리: 팝업 닫기 전에 모든 타이머 해제.                                                                                              

     ---                                                                                                                                           
     📌 13. Open Questions (구현 전 확인 필요)

     1. Daily Limit: 하루에 몇 번까지 플레이 가능한가? (무제한 vs 1회)                                                                             
     2. Retry Logic: 사용자가 특정 문제를 재시도할 수 있는가?                                                                                      
     3. Skip Behavior: Skip 시 점수를 0점으로 처리하는가, 아니면 평균 계산에서 제외하는가?                                                         
     4. Individual Feedback: 각 문제마다 점수를 표시하는가, 아니면 마지막에만 표시하는가?                                                          
     5. Audio Caching: TTS 음성을 UAudioCacheManager에 캐싱하는가?                                                                                 
     6. Recording Cleanup: 저장된 녹음 파일을 자동으로 삭제하는가? (세션 종료 시 or 일정 시간 후)                                                  

     ---
문서 버전: 1.3
최종 수정: 2025-12-27
작성자: Claude (Onepiece 프로젝트 분석 기반)
참조 문서: Documents/DevLog/AgentLog/KLingo_InterviewHello.md

**버전 1.3 변경사항:**
- **PlayerActor VoiceConversationSystem 연동 방식 수정**
- 팝업에서 직접 델리게이트 등록 제거 (잘못된 설계)
- PopupManager를 통한 올바른 라우팅 구조 적용
- OnVoiceSystemRecordingComplete → HandleRecordingComplete로 변경
- UPopupManager::RouteRecordingData() 메서드 추가

**버전 1.2 변경사항:**
- **결과 패널을 별도 팝업(UPopup_DailyResult)으로 분리**
- UPopup_DailyStudy에서 결과 표시 관련 코드 제거
- UPopup_DailyResult.h, .cpp 추가
- ShowResultPanel() → ShowResultPopup()으로 변경

**버전 1.1 변경사항:**
- 일일 최고 득점 표시 기능 추가 (Txt_BestScore 위젯)
- UConfigLibrary를 통한 최고 득점 저장/로드 로직 추가
- PlayerActor VoiceConversationSystem 연동 섹션 추가
- NativeDestruct 메서드 추가 (타이머 및 델리게이트 정리)       