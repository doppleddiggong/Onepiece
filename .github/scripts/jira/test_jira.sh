#!/bin/bash
# Jira 연동 기능 로컬 테스트 스크립트

echo "=== Jira Integration Test ==="
echo ""

# 환경 변수 설정
export JIRA_SITE="jeonminwoo0625-1762930390335"
export JIRA_EMAIL="ju100.bae@gmail.com"
export JIRA_PROJECT="UN"

# JIRA_API_TOKEN 입력 받기
echo "Please enter your JIRA_API_TOKEN:"
read -s JIRA_API_TOKEN
export JIRA_API_TOKEN

echo ""
echo "OPENAI_API_KEY is optional (press Enter to skip AI analysis):"
read -s OPENAI_API_KEY
export OPENAI_API_KEY

echo ""
echo "Starting test..."
echo ""

# 테스트용 DevLog 파일 생성 (오늘 날짜)
TODAY=$(date +%Y-%m-%d)
DEVLOG_DIR="Documents/DevLog/Daily"
DEVLOG_FILE="$DEVLOG_DIR/$TODAY.md"

mkdir -p "$DEVLOG_DIR"

if [ ! -f "$DEVLOG_FILE" ]; then
    echo "Creating test DevLog file: $DEVLOG_FILE"
    cat > "$DEVLOG_FILE" << 'EOF'
# Daily DevLog - Test

## 작업 내용
- Jira 연동 기능 테스트

## 커밋 로그
- test: Jira integration test

EOF
fi

# Jira 진척도 삽입 실행
echo "Running insert_progress.py..."
python scripts/jira/insert_progress.py

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Test completed successfully!"
    echo ""
    echo "Check the following files:"
    echo "  - $DEVLOG_FILE"
    echo "  - Documents/DevLog/Metrics/jira_issues.json"
    echo "  - Documents/DevLog/Metrics/jira_progress.json"
else
    echo ""
    echo "❌ Test failed. Check error messages above."
fi
