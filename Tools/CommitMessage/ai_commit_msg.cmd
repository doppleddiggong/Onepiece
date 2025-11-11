@echo off
setlocal enabledelayedexpansion
for /f "delims=" %%i in ('git rev-parse --show-toplevel 2^>nul') do set REPO=%%i
if not defined REPO set REPO=%~dp0\..\..\..
pushd "%REPO%" >nul 2>nul
set PY="%REPO%\Tools\CommitMessage\venv\Scripts\python.exe"
set AI_SCRIPT="%REPO%\Tools\CommitMessage\ai_gen_commit_msg.py"
set LOCAL_SCRIPT="%REPO%\Tools\CommitMessage\gen_commit_msg.py"

if exist %AI_SCRIPT% (
  %PY% %AI_SCRIPT%
) else (
  %PY% %LOCAL_SCRIPT% -MsgPath "%REPO%\Tools\CommitMessage\commit_message.txt" -ToClipboard
)
set EC=%ERRORLEVEL%
if %EC% EQU 0 (
  echo [OK] commit_message.txt 생성 완료
) else (
  echo [ERROR] 커밋 메시지 생성 실패 (%EC%)
)
popd >nul 2>nul
exit /b %EC%
