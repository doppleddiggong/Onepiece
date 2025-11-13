#!/usr/bin/env python3
"""
Unified Daily DevLog Builder

This script combines the functionality of:
- generate_daily.py: Collects git stats and renders a base template.
- generate_gpt_feedback.py: Generates developer growth feedback.
- update_devlog.py: Links meeting notes and other metrics.

It performs all operations and writes the final .md and .metrics.json files once.
"""

import argparse
import subprocess
import json
import re
import os
from pathlib import Path
from datetime import datetime, timedelta, timezone
from collections import Counter
from typing import Dict, List, Optional, Tuple

from jinja2 import Template


def sh(cmd: str) -> str:
    """Executes a shell command and returns its output."""
    try:
        return subprocess.check_output(cmd, shell=True, text=True, errors="ignore").strip()
    except subprocess.CalledProcessError:
        return ""


def get_git_stats(since: str, until: str) -> Dict:
    """Collects detailed statistics for commits within a given time range."""
    commits_hashes = sh(f'git log --since="{since}" --until="{until}" --pretty=format:"%H"').splitlines()
    if not commits_hashes:
        return {
            "count": 0, "details": [], "added": 0, "deleted": 0,
            "hotspot_files": [], "files_changed": 0, "authors": [], "commit_types": {}
        }

    added = deleted = 0
    files_touched = set()
    authors = set()
    commit_types = Counter()
    details = []

    for h in commits_hashes:
        meta = sh(f'git show --format="%s||%an" --no-patch {h}')
        subject, author = (meta.split("||", 1) + ["Unknown"])[:2]
        authors.add(author)

        numstat = sh(f'git show --numstat --format="" {h}')
        for ln in numstat.splitlines():
            m = re.match(r"(\d+|-)\s+(\d+|-)\s+(.+)", ln)
            if m:
                a, d, f = m.groups()
                if a.isdigit(): added += int(a)
                if d.isdigit(): deleted += int(d)
                files_touched.add(f)

        commit_type_match = re.match(r"^(feat|fix|docs|style|refactor|test|chore|perf)", subject.lower())
        commit_type = commit_type_match.group(1) if commit_type_match else "other"
        commit_types[commit_type] += 1
        details.append({"type": commit_type, "subject": subject})

    return {
        "count": len(commits_hashes), "details": details, "added": added, "deleted": deleted,
        "files_changed": len(files_touched), "authors": sorted(list(authors)),
        "commit_types": dict(commit_types)
    }


def extract_top_changes(details: List[Dict], max_count: int = 3) -> List[Dict]:
    """Extracts the most significant changes from commit details."""
    priority = {"feat": 1, "fix": 2, "refactor": 3, "perf": 4}
    sorted_details = sorted(details, key=lambda x: priority.get(x["type"], 99))
    
    impact_map = {
        "feat": "기능 추가", "fix": "버그 수정", "refactor": "코드 개선",
        "perf": "성능 향상", "docs": "문서 업데이트", "test": "테스트 추가",
        "chore": "유지보수", "style": "코드 스타일", "other": "기타 변경"
    }

    return [{
        "type": item["type"],
        "summary": item["subject"],
        "impact": impact_map.get(item["type"], "기타 변경")
    } for item in sorted_details[:max_count]]


def get_commit_diffs_for_gpt(since: str) -> str:
    """Gets commit diff stats for GPT analysis."""
    hashes = sh(f'git log --since="{since}" --pretty=format:"%H"').splitlines()
    if not hashes:
        return "분석할 커밋이 없습니다."

    summary = []
    for h in hashes[:5]:  # Limit to 5 commits for brevity
        subject = sh(f'git show --format="%s" --no-patch {h}')
        diff_stat = sh(f'git show --stat {h}')
        summary.append(f"**커밋 {h[:7]}**: {subject}\n{diff_stat[:300]}")
    
    return "\n\n".join(summary)


def call_gpt(system_prompt: str, user_prompt: str, model: str = "gpt-4o-mini") -> str:
    """Calls OpenAI API and returns the response."""
    api_key = os.getenv("OPENAI_API_KEY")
    if not api_key:
        return "GPT 분석을 건너뛰었습니다 (OPENAI_API_KEY가 설정되지 않음)."
    
    try:
        from openai import OpenAI
        client = OpenAI(api_key=api_key)
        response = client.chat.completions.create(
            model=model,
            messages=[
                {"role": "system", "content": system_prompt},
                {"role": "user", "content": user_prompt},
            ],
            temperature=0.5,
            max_tokens=2000
        )
        return response.choices[0].message.content.strip()
    except Exception as e:
        return f"GPT API 호출에 실패했습니다: {e}"


def generate_growth_feedback(base_devlog_content: str, since: str) -> str:
    """Generates developer growth feedback using GPT."""
    commit_diffs = get_commit_diffs_for_gpt(since)
    
    system_prompt = Path(__file__).parent / "prompt_templates" / "gpt_feedback_system.txt"
    user_prompt_template = Path(__file__).parent / "prompt_templates" / "gpt_feedback_user.txt"

    user_prompt = user_prompt_template.read_text(encoding="utf-8").format(
        devlog_content=base_devlog_content[:3000],
        commit_summary=commit_diffs
    )

    feedback = call_gpt(system_prompt.read_text(encoding="utf-8"), user_prompt, model="gpt-4o")
    
    return f"""
---

# 🎓 개발자 성장 피드백 (GPT-4 Analysis)

{feedback}

---

*이 피드백은 OpenAI GPT-4를 통해 자동 생성되었습니다. 참고용으로 활용하시고, 최종 판단은 개발자 본인이 내리시기 바랍니다.*
"""


def get_meeting_analysis(target_date: datetime, top_changes_formatted: str) -> str:
    """Generates meeting link analysis using GPT."""
    meeting_date = target_date - timedelta(days=1)
    meeting_file = Path(f"Documents/Meeting/Daily/Meeting_{meeting_date.strftime('%y%m%d')}_Daily.md")
    
    if not meeting_file.exists():
        # Fallback for different naming conventions if needed
        matches = list(Path("Documents/Meeting/Daily").glob(f"Meeting_{meeting_date.strftime('%y%m%d')}*.md"))
        if matches:
            meeting_file = matches[0]
        else:
            return "이전 날짜의 회의록을 찾을 수 없습니다."

    meeting_content = meeting_file.read_text(encoding="utf-8")
    keywords_match = re.search(r"- 키워드: (.+)", meeting_content)
    keywords = keywords_match.group(1).strip() if keywords_match else "키워드 없음"
    actions = "\n".join(re.findall(r"- \[ \] (.+)", meeting_content))

    prompt_template = Path(__file__).parent / "prompt_templates" / "meeting_link.txt"
    prompt = prompt_template.read_text(encoding="utf-8").format(
        meeting_date=meeting_date.strftime("%Y-%m-%d"),
        keywords=keywords,
        actions=actions,
        metrics_top=top_changes_formatted
    )

    return call_gpt("You are a concise DevLog summarizer.", prompt)


def main():
    parser = argparse.ArgumentParser(description="Unified Daily DevLog Builder")
    parser.add_argument("--date", required=True, help="Target date (YYYY-MM-DD)")
    parser.add_argument("--branch", required=True, help="Current git branch")
    parser.add_argument("--use-gpt", action="store_true", help="Enable GPT-based analysis")
    args = parser.parse_args()

    target_date = datetime.strptime(args.date, "%Y-%m-%d")
    since = f"{args.date} 00:00"
    until = f"{args.date} 23:59"

    # 1. Collect Data
    print("📊 Git 통계 수집 중...")
    git_stats = get_git_stats(since, until)
    top_changes = extract_top_changes(git_stats["details"])

    # 2. Prepare context for Jinja2 template
    weekday_names = ["월", "화", "수", "목", "금", "토", "일"]
    context = {
        "date": args.date,
        "weekday": weekday_names[target_date.weekday()],
        "branch": args.branch,
        "commit_count": git_stats["count"],
        "added": git_stats["added"],
        "deleted": git_stats["deleted"],
        "top_changes": top_changes,
        "generation_time": datetime.now(timezone(timedelta(hours=9))).strftime("%Y-%m-%d %H:%M:%S KST")
    }

    # 3. Render base DevLog from template
    print("📝 기본 DevLog 템플릿 렌더링 중...")
    template_path = Path(__file__).parent / "daily_template.md"
    template = Template(template_path.read_text(encoding="utf-8"))
    base_md_content = template.render(**context)

    # 4. (Optional) Generate GPT content
    growth_feedback_content = ""
    meeting_analysis_content = ""

    if args.use_gpt:
        print("🤖 GPT 분석 생성 중 (성장 피드백 및 회의록 연계)...")
        top_changes_str = "\n".join([f"- {c['summary']}" for c in top_changes])
        
        # Meeting analysis
        meeting_analysis_content = get_meeting_analysis(target_date, top_changes_str)
        
        # Growth feedback
        growth_feedback_content = generate_growth_feedback(base_md_content, since)

    # 5. Combine all parts
    final_md_content = base_md_content
    
    # Inject meeting analysis into the correct section
    meeting_section_header = "## 3. 회의 연계 분석"
    if meeting_analysis_content:
        if meeting_section_header in final_md_content:
             final_md_content = re.sub(
                rf"({meeting_section_header}\n)[\s\S]*?(\n## |$)",
                f"\\1{meeting_analysis_content}\n\n\\2",
                final_md_content,
                count=1
            )
        else:
            final_md_content += f"\n{meeting_section_header}\n{meeting_analysis_content}\n"

    # Append growth feedback at the end
    if growth_feedback_content:
        final_md_content += growth_feedback_content

    # 6. Write final .md file
    output_md_path = Path(f"Documents/DevLog/Daily/{args.date}.md")
    output_md_path.parent.mkdir(parents=True, exist_ok=True)
    output_md_path.write_text(final_md_content, encoding="utf-8")
    print(f"✅ Daily DevLog 생성 완료: {output_md_path}")

    # 7. Generate and write .metrics.json file
    metrics_data = {
        "date": args.date,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "period": {"since": since, "until": until},
        "branch": args.branch,
        "commit_count": git_stats["count"],
        "author_count": len(git_stats["authors"]),
        "authors": git_stats["authors"],
        "commit_types": git_stats["commit_types"],
        "additions": git_stats["added"],
        "deletions": git_stats["deleted"],
        "files_changed": git_stats["files_changed"],
        "top_changes": top_changes,
    }
    output_metrics_path = output_md_path.with_suffix(".metrics.json")
    output_metrics_path.write_text(json.dumps(metrics_data, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"✅ Metrics JSON 생성 완료: {output_metrics_path}")

if __name__ == "__main__":
    main()