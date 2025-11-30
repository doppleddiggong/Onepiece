# DPI 스케일링 적용 가이드

## 1) YiSan에서 확인된 DPI 설정
- 설정 파일: `Config/DefaultEngine.ini`  
  섹션 `[Script/Engine.UserInterfaceSettings]`에서 `UIScaleRule=ScaleToFit` 만 명시되어 있습니다.
- 추가 DPI 항목(`DesignScreenSize`, `ApplicationScale`, `UIScaleCurve`, 커스텀 스케일 클래스)은 작성되지 않아 **엔진 기본 DPI 스케일링 곡선과 기본 디자인 해상도**를 그대로 사용합니다.
- `ScaleToFit` 의미: 실제 해상도 대비 디자인 해상도의 가로/세로 비율 중 더 작은 값을 선택해 전체 UI 스케일에 적용합니다. 화면비가 달라도 UI가 잘리지 않도록 보수적으로 축소·확대되는 방식입니다.
- 원본 설정 스니펫:

```
[/Script/Engine.UserInterfaceSettings]
UIScaleRule=ScaleToFit
```

## 2) 다른 프로젝트에 동일하게 적용하는 방법
1. **에디터에서 설정**  
   - Edit → Project Settings → Engine → User Interface → DPI Scaling 이동.  
   - `DPI Scale Rule`을 `Scale To Fit`으로 변경하고 저장합니다.  
   - 엔진 기본값에 의존하고 싶지 않다면 `Design Screen Size`를 팀의 UI 기준 해상도(예: 1920×1080)로 함께 지정해 두는 것을 권장합니다.
2. **INI로 직접 지정 (형상관리용)**  
   - 대상 프로젝트의 `Config/DefaultEngine.ini`에 아래 블록을 추가하거나 덮어씁니다.

   ```
   [/Script/Engine.UserInterfaceSettings]
   UIScaleRule=ScaleToFit
   ; 필요 시 명시적으로 고정
   ;ApplicationScale=1.0
   ;DesignScreenSize=(X=1920,Y=1080)
   ```

3. **커브/커스텀 스케일 사용 여부 점검**  
   - 기존에 `UIScaleCurve=`나 `CustomScalingRuleClass=`가 기록되어 있다면 제거하거나 주석 처리해 YiSan과 동일한 동작을 확보합니다.
   - CommonUI 등 별도 프레임워크를 쓰는 경우에도 위 값이 우선 적용되므로, 중복 스케일이 걸리지 않도록 확인합니다.

## 3) 동작 검증 체크리스트
- 여러 해상도(예: 1280×720, 1920×1080, 2560×1440)에서 PIE 실행 후 **Window → Developer Tools → Widget Reflector**를 열어 `Application Scale`이 해상도에 따라 변하는지 확인합니다.
- 해상도 변경 시 UI가 프레임 밖으로 밀리거나 과도하게 커지는 위젯이 없는지 HUD/메인 메뉴 순으로 점검합니다.
- 필요하면 `DesignScreenSize`를 명시해 팀이 사용하는 UI 원본 해상도와 일치시킨 뒤 다시 확인합니다.
