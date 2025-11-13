#!/usr/bin/env python3
"""
Update Daily/Weekly DevLog Markdown in place using metrics.json + meeting notes + optional GPT synthesis.

Usage:
  python update_devlog.py --mode daily --date 2025-11-12
  python update_devlog.py --mode weekly --range 2025-W46
"""
import argparse
import json
import os
import re
import shutil
from pathlib import Path
from datetime import datetime, timedelta, timezone
from typing import Dict, Optional, Sequence, Tuple

PROMPT_TEMPLATES = {
    "daily": Path(__file__).parent / "prompt_templates" / "daily.txt",
    "weekly": Path(__file__).parent / "prompt_templates" / "weekly.txt",
    "meeting_link": Path(__file__).parent / "prompt_templates" / "meeting_link.txt",
}


def read_prompt(name: str, context: Dict[str, str]) -> str:
    path = PROMPT_TEMPLATES[name]
    template = path.read_text(encoding="utf-8")
    return template.format(**context)


def call_gpt(prompt: str) -> str:
    key = os.environ.get("OPENAI_API_KEY")
    if not key:
        return "GPT summary not available (OPENAI_API_KEY missing)."
    try:
        import openai

        openai.api_key = key
        response = openai.ChatCompletion.create(
            model="gpt-4o-mini",
            messages=[
                {"role": "system", "content": "You are a concise DevLog summarizer."},
                {"role": "user", "content": prompt},
            ],
            temperature=0.2,
        )
        return response.choices[0].message.content.strip()
    except Exception as exc:
        return f"GPT request failed: {exc}"


def section_bounds(text: str, header: str) -> Optional[Tuple[int, int]]:
    pattern = re.compile(rf"(## {re.escape(header)}\n)", re.MULTILINE)
    match = pattern.search(text)
    if not match:
        return None
    start = match.end()
    next_header = re.search(r"\n## ", text[start:])
    end = start + next_header.start() if next_header else len(text)
    return start, end


def update_section(md: str, header: str, content: str) -> str:
    bounds = section_bounds(md, header)
    if bounds:
        start, end = bounds
        return md[:start] + content.strip() + "\n\n" + md[end:]
    else:
        return md.strip() + f"\n\n## {header}\n{content.strip()}\n"


def extract_meeting_keywords(path: Path) -> Tuple[str, str]:
    if not path.exists():
        return "", ""
    text = path.read_text(encoding="utf-8")
    keyword_match = re.search(r"- 키워드: (.+)", text)
    if keyword_match:
        keywords = keyword_match.group(1).strip()
    else:
        keywords = ""
    actions = "\n".join(re.findall(r"- \[ \] (.+)", text))
    return keywords, actions


def ensure_metrics_file(primary: Path, fallbacks: Sequence[Path]) -> Path:
    """Ensure metrics file exists by copying from known fallbacks if needed."""

    if primary.exists():
        return primary

    for candidate in fallbacks:
        if candidate.exists():
            primary.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(candidate, primary)
            print(f"Synced metrics from fallback: {candidate} -> {primary}")
            return primary

    fallback_list = ", ".join(str(p) for p in fallbacks)
    raise FileNotFoundError(
        f"Metrics file not found: {primary} (checked fallbacks: {fallback_list})"
    )


def resolve_meeting_file(meeting_key: str) -> Optional[Path]:
    """Resolve meeting markdown path supporting legacy Meeting_YYMMDD naming."""

    canonical = Path(f"Documents/Meeting/{meeting_key}.md")
    if canonical.exists():
        return canonical

    meeting_dir = Path("Documents/Meeting")
    if not meeting_dir.exists():
        return None

    try:
        short_key = datetime.strptime(meeting_key, "%Y-%m-%d").strftime("%y%m%d")
    except ValueError:
        return None

    matches = sorted(meeting_dir.glob(f"Meeting_{short_key}_*.md"))
    return matches[0] if matches else None


def load_metrics(path: Path) -> Dict:
    with path.open(encoding="utf-8") as fh:
        return json.load(fh)


def format_top_changes(top_changes: list) -> str:
    if not top_changes:
        return "데이터 없음"
    lines = []
    for change in top_changes:
        summary = change.get("summary") or change.get("title")
        impact = change.get("impact", "영향 정보 없음")
        files = change.get("files", [])
        lines.append(f"- {summary} — {impact} (files: {len(files)})")
    return "\n".join(lines)


def format_impact(metrics: Dict) -> str:
    parts = []
    for field in ("ubt", "cook", "tests", "static_analysis"):
        value = metrics.get(field)
        if value is not None:
            parts.append(f"- {field}: {value}")
    return "\n".join(parts) if parts else "데이터 없음"


def format_mermaid(metrics: Dict) -> str:
    commits = metrics.get("commit_count", 0)
    builds = metrics.get("ubt_runs", 0)
    tests = metrics.get("tests", 0)
    docs = metrics.get("docs", 0)
    return (
        f"Commits: {commits}\n"
        f"Builds: {builds}\n"
        f"Tests: {tests}\n"
        f"Docs: {docs}\n"
    )


def update_markdown(
    target: Path,
    metrics: Dict,
    section_summary: str,
    meeting_link: str,
    generated_at: str,
) -> None:
    if target.exists():
        data = target.read_text(encoding="utf-8")
    else:
        data = "# Daily DevLog\n\n"
    data = update_section(data, "1. 오늘의 핵심 변경", section_summary)
    data = update_section(data, "2. 시스템 영향도", format_impact(metrics))
    data = update_section(data, "4. Mermaid 개요도", format_mermaid(metrics))
    data = update_section(data, "생성 시간", generated_at)
    if meeting_link:
        data = update_section(data, "3. 회의 연계 분석", meeting_link)
    target.write_text(data.strip() + "\n", encoding="utf-8")


def build_generated_at() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--mode", choices=["daily", "weekly"], required=True)
    parser.add_argument("--date", help="YYYY-MM-DD for daily")
    parser.add_argument("--range", help="YYYY-W## for weekly")
    parser.add_argument("--meeting-date", help="YYYY-MM-DD of meeting note")
    args = parser.parse_args()

    if args.mode == "daily":
        if not args.date:
            raise ValueError("daily mode requires --date")
        metrics_primary = Path(f"Documents/DevLog/Metrics/{args.date}.json")
        fallback_candidates = [Path(f"Documents/DevLog/Daily/{args.date}.metrics.json")]
        metrics_path = ensure_metrics_file(metrics_primary, fallback_candidates)
        target = Path(f"Documents/DevLog/Daily/{args.date}.md")
        meeting_key = args.meeting_date or (datetime.strptime(args.date, "%Y-%m-%d") - timedelta(days=1)).strftime("%Y-%m-%d")
    else:
        if not args.range:
            raise ValueError("weekly mode requires --range")
        metrics_primary = Path(f"Documents/DevLog/Metrics/{args.range}.json")
        fallback_candidates = [Path(f"Documents/DevLog/Weekly/{args.range}.metrics.json")]
        metrics_path = ensure_metrics_file(metrics_primary, fallback_candidates)
        target = Path(f"Documents/DevLog/Weekly/{args.range}.md")
        meeting_key = args.meeting_date or None

    metrics = load_metrics(metrics_path)
    top_changes = format_top_changes(metrics.get("top_changes", []))
    summary_context = {
        "date": args.date or args.range,
        "metrics_summary": metrics,
        "top_changes": top_changes,
    }

    prompt_text = read_prompt(args.mode, summary_context)
    gpt_summary = call_gpt(prompt_text)

    meeting_link = ""
    if meeting_key:
        meeting_file = resolve_meeting_file(meeting_key)
        if meeting_file:
            keywords, actions = extract_meeting_keywords(meeting_file)
            meeting_context = {
                "meeting_date": meeting_key,
                "keywords": keywords,
                "actions": actions,
                "metrics_top": top_changes,
            }
            link_prompt = read_prompt("meeting_link", meeting_context)
            meeting_link = call_gpt(link_prompt)
        else:
            print(f"No meeting note found for key {meeting_key}")

    generated = f"생성 시간: {build_generated_at()}"
    update_markdown(target, metrics, gpt_summary, meeting_link, generated)
    print(f"Updated {target}")


if __name__ == "__main__":
    main()
