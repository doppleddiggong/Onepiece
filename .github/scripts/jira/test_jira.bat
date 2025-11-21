@echo off
REM Jira 연동 기능 로컬 테스트 스크립트 (Windows)

echo === Jira Integration Test ===
echo.

REM 환경 변수 설정
set JIRA_SITE=jeonminwoo0625-1762930390335
set JIRA_EMAIL=ju100.bae@gmail.com
set JIRA_PROJECT=UN

REM JIRA_API_TOKEN 입력 받기
set /p JIRA_API_TOKEN="Please enter your JIRA_API_TOKEN: "

REM OPENAI_API_KEY 입력 받기 (선택사항)
echo.
set /p OPENAI_API_KEY="OPENAI_API_KEY (optional, press Enter to skip): "

echo.
echo Starting test...
echo.

REM 테스트 날짜 설정 (어제 날짜)
for /f %%i in ('python -c "import datetime; print((datetime.date.today() - datetime.timedelta(days=1)).strftime(\"%%Y-%%m-%%d\"))"') do set TEST_DATE=%%i
set DEVLOG_DIR=Documents\DevLog\Daily
set DEVLOG_FILE=%DEVLOG_DIR%\%TEST_DATE%.md

echo.
echo Target Date: %TEST_DATE%
echo Target File: %DEVLOG_FILE%
echo.

REM 실제 DevLog 생성 (build_daily_log.py 실행)
echo Step 1: Generating Daily DevLog using build_daily_log.py...
python .github/scripts/devlog/build_daily_log.py --date %TEST_DATE% --branch main

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [WARNING] DevLog generation failed or no commits found.
    echo Creating minimal test DevLog instead...
    if not exist "%DEVLOG_DIR%" mkdir "%DEVLOG_DIR%"
    python -c "import pathlib; pathlib.Path(r'%DEVLOG_FILE%').write_text('# Daily DevLog - %TEST_DATE%\n\n## 1. 오늘의 핵심 변경 (Top Changes)\n\n- 테스트용 DevLog\n\n---\n\n', encoding='utf-8')"
)

echo.
echo Step 2: Inserting Jira progress into DevLog...
python .github\scripts\jira\insert_progress.py

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Test completed successfully!
    echo.
    echo Check the following files:
    echo   - %DEVLOG_FILE%
    echo   - Documents\DevLog\Metrics\jira_issues.json
    echo   - Documents\DevLog\Metrics\jira_progress.json
) else (
    echo.
    echo [FAILED] Test failed. Check error messages above.
)

pause
