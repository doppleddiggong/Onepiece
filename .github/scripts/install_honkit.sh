#!/bin/bash
# HonKit DevLog 자동화 시스템 설치 스크립트

set -e

echo "🚀 HonKit DevLog 자동화 시스템 설치를 시작합니다..."
echo ""

# 프로젝트 정보 입력
read -p "📝 프로젝트명을 입력하세요: " PROJECT_NAME
read -p "📝 프로젝트 설명을 입력하세요: " PROJECT_DESC
read -p "📝 작성자명을 입력하세요: " AUTHOR_NAME
read -p "📝 GitHub 사용자명을 입력하세요: " GITHUB_USER
read -p "📝 GitHub 저장소명을 입력하세요: " GITHUB_REPO

echo ""
echo "입력된 정보:"
echo "  프로젝트명: $PROJECT_NAME"
echo "  설명: $PROJECT_DESC"
echo "  작성자: $AUTHOR_NAME"
echo "  GitHub: https://github.com/$GITHUB_USER/$GITHUB_REPO"
echo ""
read -p "계속하시겠습니까? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "❌ 설치를 취소했습니다."
    exit 1
fi

# 1. 디렉토리 구조 생성
echo "📁 디렉토리 구조 생성 중..."
mkdir -p Documents/{DevLog,Planning,SystemReview}
mkdir -p .github/{workflows,scripts/devlog}
echo "✅ 디렉토리 구조 생성 완료"

# 2. HonKit 설정 파일 생성
echo "⚙️  HonKit 설정 파일 생성 중..."

cat > Documents/book.json <<EOF
{
  "title": "$PROJECT_NAME Development Documentation",
  "description": "$PROJECT_DESC",
  "author": "$AUTHOR_NAME",
  "language": "ko",
  "gitbook": "3.2.3",
  "structure": {
    "readme": "README.md",
    "summary": "SUMMARY.md"
  },
  "plugins": [
    "github",
    "mermaid-gb3",
    "collapsible-menu",
    "back-to-top-button",
    "search-plus",
    "expandable-chapters-small",
    "-lunr",
    "-search"
  ],
  "pluginsConfig": {
    "github": {
      "url": "https://github.com/$GITHUB_USER/$GITHUB_REPO"
    },
    "mermaid-gb3": {
      "theme": "default"
    }
  },
  "links": {
    "sidebar": {
      "GitHub Repository": "https://github.com/$GITHUB_USER/$GITHUB_REPO"
    }
  }
}
EOF

cat > Documents/README.md <<EOF
# $PROJECT_NAME Development Documentation

환영합니다! $PROJECT_NAME 프로젝트의 개발 문서 사이트입니다.

## 문서 구조

### 📅 DevLog (개발 일지)
- **Daily**: 일별 개발 로그 (자동 생성)
- **Weekly**: 주간 요약 및 회고
- **Monthly**: 월간 리포트

### 📋 Planning (기획 문서)
- 시스템 설계 문서
- 기능 명세서
- API 문서

### 🔍 System Review
- 주간/월간 시스템 리뷰
- 아키텍처 변화 분석
- 성능 및 안정성 메트릭

## 관련 링크

- **GitHub Repository**: [$GITHUB_USER/$GITHUB_REPO](https://github.com/$GITHUB_USER/$GITHUB_REPO)

---

**마지막 업데이트**: $(date +%Y-%m-%d)
EOF

cat > Documents/SUMMARY.md <<EOF
# Summary

## Introduction
* [시작하기](README.md)

## DevLog

### Daily Logs
(자동으로 업데이트됩니다)

### Weekly Summary
(자동으로 업데이트됩니다)

## Planning

(문서를 추가하면 자동으로 업데이트됩니다)

## External Links
* [GitHub Repository](https://github.com/$GITHUB_USER/$GITHUB_REPO)
EOF

echo "✅ HonKit 설정 파일 생성 완료"

# 3. Python 스크립트 복사 안내
echo ""
echo "⚠️  Python 스크립트를 수동으로 복사해야 합니다:"
echo "   원본 위치: YiSan/.github/scripts/devlog/"
echo "   대상 위치: .github/scripts/devlog/"
echo ""
echo "   필요한 파일:"
echo "   - generate_daily.py"
echo "   - daily_template.md"
echo "   - generate_weekly.py"
echo "   - weekly_template.md"
echo "   - generate_system_review.py"
echo "   - system_review_template.md"
echo "   - update_summary.py"
echo "   - send_discord.py"
echo ""
read -p "스크립트를 복사했습니까? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "⚠️  나중에 스크립트를 복사해주세요."
fi

# 4. GitHub Actions 워크플로우 안내
echo ""
echo "⚠️  GitHub Actions 워크플로우를 수동으로 복사해야 합니다:"
echo "   원본 위치: YiSan/.github/workflows/"
echo "   대상 위치: .github/workflows/"
echo ""
echo "   필요한 파일:"
echo "   - honkit.yml"
echo "   - devlog-simple.yml"
echo "   - weekly-report.yml"
echo "   - system-review.yml"
echo ""
read -p "워크플로우를 복사했습니까? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "⚠️  나중에 워크플로우를 복사해주세요."
fi

# 5. Git 설정
echo ""
echo "📝 Git 설정..."
if [ -d .git ]; then
    git add Documents/ .github/
    echo "✅ Git에 파일이 추가되었습니다."
else
    echo "⚠️  Git 저장소가 아닙니다. 먼저 'git init'을 실행하세요."
fi

# 6. 다음 단계 안내
echo ""
echo "🎉 기본 설정이 완료되었습니다!"
echo ""
echo "📋 다음 단계:"
echo ""
echo "1. GitHub Pages 활성화"
echo "   - Repository Settings → Pages"
echo "   - Source: Deploy from a branch"
echo "   - Branch: gh-pages / / (root)"
echo ""
echo "2. (선택) Discord Webhook 설정"
echo "   - Discord에서 Webhook URL 생성"
echo "   - GitHub Repository Settings → Secrets → New secret"
echo "   - Name: DISCORD_WEBHOOK_URL"
echo "   - Value: (Webhook URL)"
echo ""
echo "3. 첫 커밋 및 푸시"
echo "   git commit -m 'chore: setup HonKit DevLog automation'"
echo "   git push"
echo ""
echo "4. GitHub Actions에서 워크플로우 확인"
echo "   https://github.com/$GITHUB_USER/$GITHUB_REPO/actions"
echo ""
echo "5. 문서 사이트 확인"
echo "   https://$GITHUB_USER.github.io/$GITHUB_REPO/docs/"
echo ""
echo "✨ 설치가 완료되었습니다!"
