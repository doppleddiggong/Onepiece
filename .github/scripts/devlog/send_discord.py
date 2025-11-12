#!/usr/bin/env python3
"""
Discord Webhook Sender
DevLog를 Discord로 전송하고 피드백을 유도합니다.
"""

import argparse
import json
import re
import os
import subprocess
from pathlib import Path
from datetime import datetime

def send_webhook(webhook_url, payload):
    """Discord Webhook으로 메시지 전송 (curl 사용)"""
    if not webhook_url:
        print("⚠️ Discord webhook URL이 설정되지 않았습니다.")
        return False

    try:
        # curl을 사용하여 전송 (GitHub Actions에서 안정적)
        result = subprocess.run(
            [
                'curl',
                '-H', 'Content-Type: application/json',
                '-X', 'POST',
                '-d', json.dumps(payload),
                webhook_url
            ],
            capture_output=True,
            text=True,
            check=True
        )

        print("✅ Discord 메시지 전송 성공")
        return True

    except subprocess.CalledProcessError as e:
        print(f"❌ HTTP Error: curl failed with exit code {e.returncode}")
        if e.stderr:
            print(f"   Error: {e.stderr}")
        if e.stdout:
            print(f"   Response: {e.stdout}")
        return False
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def extract_summary_from_daily(md_path):
    """Daily DevLog에서 요약 정보 추출"""
    content = Path(md_path).read_text(encoding='utf-8')

    summary = {}

    # 커밋 수 추출
    match = re.search(r'총 커밋:\s*(\d+)', content)
    if match:
        summary['commits'] = int(match.group(1))

    # 변경 라인 추출
    match = re.search(r'변경 라인:\s*\+(\d+)\s*/\s*-(\d+)', content)
    if match:
        summary['added'] = int(match.group(1))
        summary['deleted'] = int(match.group(2))

    # 핵심 변경 추출 (최대 3개)
    top_changes = []
    in_top_changes = False
    for line in content.split('\n'):
        if '## 1. 오늘의 핵심 변경' in line:
            in_top_changes = True
            continue
        if in_top_changes:
            if line.startswith('##'):
                break
            if line.startswith('- ['):
                # [type] summary 형식 파싱
                match = re.match(r'-\s*\[([^\]]+)\]\s*(.+)', line)
                if match:
                    top_changes.append({
                        'type': match.group(1),
                        'summary': match.group(2).split('—')[0].strip()
                    })
                    if len(top_changes) >= 3:
                        break

    summary['top_changes'] = top_changes

    return summary

def extract_summary_from_weekly(md_path):
    """Weekly DevLog에서 요약 정보 추출"""
    content = Path(md_path).read_text(encoding='utf-8')

    summary = {}

    # 주간 요약 추출
    match = re.search(r'\*\*주간 요약\*\*:\s*(.+)', content)
    if match:
        summary['summary'] = match.group(1).strip()

    # 총 커밋 추출
    match = re.search(r'\*\*총 커밋\*\*:\s*(\d+)개', content)
    if match:
        summary['commits'] = int(match.group(1))

    # 활동 일수 추출
    match = re.search(r'\*\*활동 일수\*\*:\s*(\d+)/7일', content)
    if match:
        summary['active_days'] = int(match.group(1))

    # 신규 기능 수 추출
    features = len(re.findall(r'###\s*신규 기능.*?\n\n(.*?)(?=###|\n##|\Z)', content, re.DOTALL))
    summary['features'] = features

    # 버그 수정 수 추출
    fixes = len(re.findall(r'###\s*버그 수정.*?\n\n(.*?)(?=###|\n##|\Z)', content, re.DOTALL))
    summary['fixes'] = fixes

    return summary

def _extract_first_text_line(content: str) -> str:
    """Extract the first meaningful line of text from markdown content."""

    cleaned_content = re.sub(r'\*\*(.*?)\*\*', r'\1', content)

    for raw_line in cleaned_content.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if line.startswith('#'):
            continue
        if line.startswith('```'):
            # Skip fenced code blocks headers
            continue
        if line.startswith('>'):
            line = line.lstrip('> ').strip()
        line = re.sub(r'^[\-*\d\.)\s]+', '', line)
        line = re.sub(r'\s+', ' ', line).strip()
        if line:
            return line

    return "자세한 내용은 문서를 확인해주세요."


def _truncate_sentence(text: str, max_length: int = 140) -> str:
    """Return the first sentence (or truncated text) within the max length."""

    sentence = re.split(r'(?<=[.!?])\s+', text)[0].strip()
    if not sentence:
        sentence = text.strip()

    if len(sentence) > max_length:
        truncated = sentence[:max_length].rstrip()
        if not truncated.endswith('…'):
            truncated += '…'
        return truncated

    return sentence


def extract_summary_from_meeting_log(md_path):
    """회의록 파일에서 제목과 요약 추출"""
    content = Path(md_path).read_text(encoding='utf-8')
    summary = {}

    # 최상단 H1을 제목으로 사용, 없으면 이전 로직으로 대체
    title_match = re.search(r'^#\s*(.+)', content, re.MULTILINE)
    if not title_match:
        title_match = re.search(r'^###\s*(.+)', content, re.MULTILINE)
    summary['title'] = title_match.group(1).strip() if title_match else md_path.stem

    # 본문에서 첫 문장을 추출하여 설명으로 사용
    first_line = _extract_first_text_line(content)
    summary['description'] = _truncate_sentence(first_line, max_length=140)

    # 커밋 메시지 추출 (환경 변수에서)
    commit_message = os.getenv('COMMIT_MESSAGE', 'N/A')
    summary['commit_message'] = commit_message.split('\n')[0]

    # 커밋 URL 추출 (환경 변수에서)
    commit_url = os.getenv('COMMIT_URL', '')
    summary['commit_url'] = commit_url

    return summary


def create_daily_embed(date, summary, devlog_url):
    """Daily DevLog용 Discord Embed 생성"""
    color = 0x5865F2  # Discord 블루

    # 핵심 변경 필드
    changes_text = ""
    if summary.get('top_changes'):
        for change in summary['top_changes'][:3]:
            emoji_map = {
                'feat': '✨',
                'fix': '🐛',
                'refactor': '♻️',
                'perf': '⚡',
                'docs': '📝',
                'test': '✅',
                'chore': '🔧'
            }
            emoji = emoji_map.get(change['type'], '📌')
            changes_text += f"{emoji} **{change['type']}**: {change['summary']}\n"

    if not changes_text:
        changes_text = "변경 사항 없음"

    # 통계 필드
    stats_text = f"📊 커밋: {summary.get('commits', 0)}개\n"
    stats_text += f"➕ 추가: {summary.get('added', 0)} 라인\n"
    stats_text += f"➖ 삭제: {summary.get('deleted', 0)} 라인"

    embed = {
        "title": f"📅 Daily DevLog — {date}",
        "description": "오늘의 개발 활동이 기록되었습니다.",
        "color": color,
        "fields": [
            {
                "name": "🎯 주요 변경 사항",
                "value": changes_text,
                "inline": False
            },
            {
                "name": "📈 통계",
                "value": stats_text,
                "inline": False
            }
        ],
        "footer": {
            "text": "💡 DevLog를 확인하고 피드백을 남겨주세요!"
        },
        "timestamp": datetime.utcnow().isoformat()
    }

    if devlog_url:
        embed["url"] = devlog_url

    return embed

def create_weekly_embed(week_label, summary, devlog_url):
    """Weekly DevLog용 Discord Embed 생성"""
    color = 0xFEE75C  # Discord 옐로우

    # 주간 요약
    summary_text = summary.get('summary', '활동 없음')

    # 통계
    stats_text = f"📊 총 커밋: {summary.get('commits', 0)}개\n"
    stats_text += f"📅 활동 일수: {summary.get('active_days', 0)}/7일\n"
    stats_text += f"✨ 신규 기능: {summary.get('features', 0)}개\n"
    stats_text += f"🐛 버그 수정: {summary.get('fixes', 0)}개"

    embed = {
        "title": f"📊 Weekly Report — {week_label}",
        "description": summary_text,
        "color": color,
        "fields": [
            {
                "name": "📈 주간 통계",
                "value": stats_text,
                "inline": False
            },
            {
                "name": "💭 회고 시간!",
                "value": "Weekly Report에 포함된 회고 질문에 답변해주세요.\n팀원들과 함께 이번 주를 돌아보고 다음 주를 계획해봅시다! 🚀",
                "inline": False
            }
        ],
        "footer": {
            "text": "✍️ 회고 질문에 답변을 작성하고 함께 성장하세요!"
        },
        "timestamp": datetime.utcnow().isoformat()
    }

    if devlog_url:
        embed["url"] = devlog_url

    return embed

def create_meeting_log_embed(summary, devlog_url, notice=None):
    """회의록 공유용 Discord Embed 생성"""
    color = 0x9B59B6  # 보라색

    description = summary.get('description') or "회의록이 업데이트되었습니다."

    if notice:
        notice_text = notice.strip()
        if notice_text:
            description = f"{notice_text}\n\n{description}"

    embed = {
        "title": f"📚 {summary.get('title', '새로운 회의록')}",
        "description": description,
        "color": color,
        "fields": [
            {
                "name": "📝 Commit Message",
                "value": summary.get('commit_message', 'N/A'),
                "inline": False
            }
        ],
        "footer": {
            "text": "자세한 내용은 링크를 클릭하여 확인하세요."
        },
        "timestamp": datetime.utcnow().isoformat()
    }

    # Honkit URL이 있으면 최우선으로 사용, 없으면 커밋 URL 사용
    if devlog_url:
        embed['url'] = devlog_url
    elif summary.get('commit_url'):
        embed['url'] = summary.get('commit_url')

    return embed


def create_feedback_thread(webhook_url, message_content):
    """피드백 스레드 메시지 생성"""
    # Discord에서는 webhook으로 직접 스레드를 만들 수 없으므로
    # 메시지에 피드백 가이드를 포함
    feedback_guide = {
        "content": message_content,
        "components": []  # Discord 봇이 필요한 부분
    }
    return feedback_guide

def main():
    ap = argparse.ArgumentParser(description="Discord Webhook Sender")
    ap.add_argument("--webhook-url", required=True, help="Discord Webhook URL")
    ap.add_argument("--type", choices=["daily", "weekly", "meeting_log"], required=True, help="리포트 또는 메시지 타입")
    ap.add_argument("--devlog-file", help="DevLog 파일 경로")
    ap.add_argument("--date", help="날짜 또는 주차 (YYYY-MM-DD, YYYY-WXX 등)")
    ap.add_argument("--devlog-url", help="DevLog 온라인 URL")
    ap.add_argument("--notice", help="추가 안내 문구 (회의록 다중 변경 등)")
    args = ap.parse_args()

    # DevLog 파일 확인
    if args.type in ["daily", "weekly", "meeting_log"]:
        if not args.devlog_file:
            print(f"❌ --type이 {args.type}일 경우 --devlog-file은 필수입니다.")
            return 1
        devlog_path = Path(args.devlog_file)
        if not devlog_path.exists():
            print(f"❌ DevLog 파일을 찾을 수 없습니다: {args.devlog_file}")
            return 1

    # 요약 정보 추출
    if args.type == "daily":
        summary = extract_summary_from_daily(devlog_path)
        embed = create_daily_embed(args.date, summary, args.devlog_url)
        username = "DevLog Bot 📅"
    elif args.type == "weekly":
        summary = extract_summary_from_weekly(devlog_path)
        embed = create_weekly_embed(args.date, summary, args.devlog_url)
        username = "Weekly Report Bot 📊"
    elif args.type == "meeting_log":
        summary = extract_summary_from_meeting_log(devlog_path)
        embed = create_meeting_log_embed(summary, args.devlog_url, notice=args.notice)
        username = "회의록 알리미 ✍️"

    # Webhook 페이로드 구성
    payload = {
        "username": username,
        "embeds": [embed]
    }

    # 전송
    success = send_webhook(args.webhook_url, payload)

    return 0 if success else 1

if __name__ == "__main__":
    exit(main())
