#!/usr/bin/env python3
"""
Daily DevLog Generator
자동으로 Git 커밋, 빌드 결과, 테스트 결과를 수집하여 일일 개발 로그를 생성합니다.
"""

import argparse
import subprocess
import json
import re
import datetime
from collections import Counter
from pathlib import Path
from jinja2 import Template

def sh(cmd):
    """셸 명령 실행"""
    try:
        return subprocess.check_output(cmd, shell=True, text=True, errors="ignore").strip()
    except subprocess.CalledProcessError:
        return ""

def git_range_since(since, until=None):
    """특정 시간 범위의 커밋 해시 목록 반환"""
    cmd = f'git log --since="{since}"'
    if until:
        cmd += f' --until="{until}"'
    cmd += ' --pretty=format:"%H"'
    result = sh(cmd)
    return result.splitlines() if result else []

def git_stats(commits):
    """커밋 통계 수집"""
    added = deleted = 0
    details = []
    hotspot_files = {}
    files_touched = set()
    authors = set()
    commit_types = Counter()

    for h in commits:
        # 커밋 메타데이터 가져오기
        meta = sh(f'git show --format="%s||%h||%an" --no-patch {h}')
        if not meta:
            continue

        subject, short, author = meta.split("||") if "||" in meta else (meta, h[:7], "Unknown")
        authors.add(author)

        # 변경 파일 통계
        numstat = sh(f'git show --numstat --format="" {h}')
        for ln in numstat.splitlines():
            m = re.match(r"(\d+|-)\s+(\d+|-)\s+(.+)", ln)
            if not m:
                continue
            a, d, f = m.groups()
            if a != '-' and d != '-':
                added += int(a)
                deleted += int(d)
                # Hotspot 파일 추적
                hotspot_files[f] = hotspot_files.get(f, 0) + int(a) + int(d)
                files_touched.add(f)

        # Conventional Commits 파싱
        commit_type = parse_commit_type(subject)
        commit_types[commit_type] += 1

        details.append({
            "hash": short,
            "subject": subject,
            "author": author,
            "type": commit_type
        })

    # 상위 5개 hotspot 파일
    top_hotspots = sorted(hotspot_files.items(), key=lambda x: x[1], reverse=True)[:5]
    hotspot_list = [f for f, _ in top_hotspots]

    return {
        "added": added,
        "deleted": deleted,
        "details": details,
        "count": len(commits),
        "hotspot_files": hotspot_list,
        "files_changed": len(files_touched),
        "authors": sorted(authors),
        "commit_types": dict(commit_types)
    }

def parse_commit_type(subject):
    """Conventional Commits에서 타입 추출"""
    match = re.match(r"^(feat|fix|docs|style|refactor|test|chore|perf)(\(.+?\))?:", subject.lower())
    if match:
        return match.group(1)
    return "other"

def parse_ubt_summary(path):
    """UnrealBuildTool 로그 파싱"""
    d = {"success": None, "target": "", "errors": 0}
    p = Path(path)
    if not p.exists():
        return d

    try:
        txt = p.read_text(errors="ignore", encoding="utf-8")
        d["success"] = ("Error" not in txt and "BUILD FAILED" not in txt)
        m = re.search(r"Building\s+(.+?)\s+-\s+(.+)", txt)
        if m:
            d["target"] = m.group(1).strip()
        d["errors"] = len(re.findall(r": error:", txt))
    except:
        pass

    return d

def parse_cook_summary(path):
    """Cook 로그 파싱"""
    d = {"cooked": 0, "skipped": 0, "errors": 0}
    p = Path(path)
    if not p.exists():
        return d

    try:
        txt = p.read_text(errors="ignore", encoding="utf-8")
        m = re.search(r"Cooked\s+(\d+)", txt)
        d["cooked"] = int(m.group(1)) if m else 0
        m = re.search(r"Skipped\s+(\d+)", txt)
        d["skipped"] = int(m.group(1)) if m else 0
        d["errors"] = len(re.findall(r"\bError\b", txt))
    except:
        pass

    return d

def parse_tests(junit_xml):
    """JUnit XML 테스트 결과 파싱"""
    p = Path(junit_xml)
    if not p.exists():
        return {"pass": None, "fail": None, "coverage": None}

    try:
        from lxml import etree
        root = etree.parse(str(p))
        total_tests = int(root.xpath("sum(//testsuite/@tests)") or 0)
        failures = int(root.xpath("sum(//testsuite/@failures)") or 0)
        errors = int(root.xpath("sum(//testsuite/@errors)") or 0)
        t_pass = total_tests - failures - errors
        t_fail = failures + errors

        cov = None
        covnode = root.xpath("//properties/property[@name='coverage']/@value")
        if covnode:
            cov = float(covnode[0])

        return {"pass": t_pass, "fail": t_fail, "coverage": cov}
    except:
        return {"pass": None, "fail": None, "coverage": None}

def parse_static(report_json):
    """정적분석 리포트 파싱"""
    p = Path(report_json)
    if not p.exists():
        return {"warn_now": None, "warn_prev": 0, "high_new": 0}

    try:
        d = json.loads(p.read_text(encoding="utf-8"))
        return {
            "warn_now": d.get("warnings", 0),
            "warn_prev": d.get("warnings_prev", 0),
            "high_new": d.get("high_new", 0)
        }
    except:
        return {"warn_now": None, "warn_prev": 0, "high_new": 0}

def load_metrics(path):
    """메트릭 JSON 로드"""
    p = Path(path)
    if p.exists():
        try:
            return json.loads(p.read_text(encoding="utf-8"))
        except:
            pass
    return {}

def extract_top_changes(details, max_count=3):
    """상위 변경 사항 추출"""
    # feat, fix, refactor 우선순위로 정렬
    priority = {"feat": 1, "fix": 2, "refactor": 3, "perf": 4}
    sorted_details = sorted(details, key=lambda x: priority.get(x["type"], 99))

    top = []
    for item in sorted_details[:max_count]:
        # PR 번호 추출
        pr_match = re.search(r"#(\d+)", item["subject"])
        pr_number = pr_match.group(1) if pr_match else None

        top.append({
            "type": item["type"],
            "summary": item["subject"],
            "pr_number": pr_number,
            "impact": get_impact_brief(item["type"])
        })

    return top

def get_impact_brief(commit_type):
    """커밋 타입에 따른 영향도 요약"""
    impact_map = {
        "feat": "기능 추가",
        "fix": "버그 수정",
        "refactor": "코드 개선",
        "perf": "성능 향상",
        "docs": "문서 업데이트",
        "test": "테스트 추가",
        "chore": "유지보수",
        "style": "코드 스타일"
    }
    return impact_map.get(commit_type, "기타 변경")

def render(context, template_path, out_path):
    """Jinja2 템플릿 렌더링"""
    template_content = Path(template_path).read_text(encoding="utf-8")
    md = Template(template_content).render(**context)

    Path(out_path).parent.mkdir(parents=True, exist_ok=True)
    Path(out_path).write_text(md, encoding="utf-8")

def main():
    ap = argparse.ArgumentParser(description="Daily DevLog Generator")
    ap.add_argument("--since", default="24 hours", help="Git log 시작 시간")
    ap.add_argument("--until", default=None, help="Git log 종료 시간 (선택)")
    ap.add_argument("--branch", required=True, help="현재 브랜치")
    ap.add_argument("--base", default="origin/main", help="베이스 브랜치")
    ap.add_argument("--out", required=True, help="출력 파일 경로")
    ap.add_argument("--template", default=None, help="템플릿 파일 경로")
    ap.add_argument("--ubt_log", default="", help="UnrealBuildTool 로그 경로")
    ap.add_argument("--cook_log", default="", help="Cook 로그 경로")
    ap.add_argument("--test_xml", default="", help="테스트 결과 XML 경로")
    ap.add_argument("--static_report", default="", help="정적분석 리포트 경로")
    ap.add_argument("--metrics", default="", help="메트릭 JSON 경로")
    ap.add_argument("--metrics-out", default="", help="메트릭 JSON 출력 경로")
    args = ap.parse_args()

    # 템플릿 경로 설정
    if not args.template:
        script_dir = Path(__file__).parent
        args.template = script_dir / "daily_template.md"

    # 데이터 수집
    commits = git_range_since(args.since, args.until)
    gstats = git_stats(commits)
    ubt = parse_ubt_summary(args.ubt_log) if args.ubt_log else {}
    cook = parse_cook_summary(args.cook_log) if args.cook_log else {}
    tests = parse_tests(args.test_xml) if args.test_xml else {}
    srep = parse_static(args.static_report) if args.static_report else {}
    met = load_metrics(args.metrics) if args.metrics else {}

    # 상위 변경 사항 추출
    top_changes = extract_top_changes(gstats["details"]) if gstats["details"] else []

    # 날짜 계산
    today = datetime.date.today()
    yesterday = today - datetime.timedelta(days=1)
    weekday_names = ["월", "화", "수", "목", "금", "토", "일"]

    # 메트릭 구성
    kst = datetime.timezone(datetime.timedelta(hours=9))
    now_kst = datetime.datetime.now(tz=kst)
    metrics_output = dict(met)
    metrics_output.update({
        "date": today.isoformat(),
        "generated_at": now_kst.isoformat(),
        "period": {
            "since": args.since,
            "until": args.until if args.until else f"{today.isoformat()} 23:59"
        },
        "branch": args.branch,
        "base_branch": args.base,
        "commit_count": gstats["count"],
        "author_count": len(gstats["authors"]),
        "authors": gstats["authors"],
        "commit_types": gstats["commit_types"],
        "additions": gstats["added"],
        "deletions": gstats["deleted"],
        "net_lines": gstats["added"] - gstats["deleted"],
        "files_changed": gstats["files_changed"],
        "hotspot_files": gstats["hotspot_files"],
        "top_changes": top_changes,
        "ubt": ubt,
        "cook": cook,
        "tests": tests,
        "static_analysis": srep,
    })

    metrics_path = Path(args.metrics_out) if args.metrics_out else Path(args.out).with_suffix(".metrics.json")
    metrics_path.parent.mkdir(parents=True, exist_ok=True)
    metrics_path.write_text(json.dumps(metrics_output, ensure_ascii=False, indent=2), encoding="utf-8")

    # 컨텍스트 구성
    ctx = {
        "date": today.isoformat(),
        "weekday": weekday_names[today.weekday()],
        "date_from": yesterday.isoformat(),
        "date_to": today.isoformat(),
        "branch": args.branch,
        "base_branch": args.base,
        "release_tag": metrics_output.get("release_tag", "N/A"),
        "commit_count": gstats["count"],
        "added": gstats["added"],
        "deleted": gstats["deleted"],
        "hotspot_files": gstats["hotspot_files"],
        "top_changes": top_changes,
        "ubt_target": ubt.get("target", ""),
        "cook_summary": f'cooked {cook.get("cooked", 0)}, skipped {cook.get("skipped", 0)}, errors {cook.get("errors", 0)}',
        "pass": tests.get("pass"),
        "fail": tests.get("fail"),
        "coverage": tests.get("coverage"),
        "warn_now": metrics_output.get("static_analysis", {}).get("warn_now"),
        "warn_prev": metrics_output.get("static_analysis", {}).get("warn_prev", 0),
        "high_new": metrics_output.get("static_analysis", {}).get("high_new", 0),
        "load_time_prev": metrics_output.get("load_time_prev"),
        "load_time_now": metrics_output.get("load_time_now"),
        "delta_pct": metrics_output.get("delta_pct"),
        "crash_prev": metrics_output.get("crash_prev"),
        "crash_now": metrics_output.get("crash_now"),
        "build_fail_count": metrics_output.get("build_fail_count", 0),
        "rtt_ms": metrics_output.get("rtt_ms"),
        "net_fail_pct": metrics_output.get("net_fail_pct"),
        "api_added": metrics_output.get("api_added", 0),
        "api_changed": metrics_output.get("api_changed", 0),
        "api_removed": metrics_output.get("api_removed", 0),
        "notable_api_list": metrics_output.get("notable_api_list", []),
        "refactor_items": metrics_output.get("refactor_items", []),
        "risk_items": metrics_output.get("risk_items", []),
        "mitigation": metrics_output.get("mitigation", ""),
        "next_items": metrics_output.get("next_items", []),
        "pending_items": metrics_output.get("pending_items", []),
        "generation_time": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    }

    # 렌더링 및 저장
    render(ctx, args.template, args.out)
    print(f"✅ Daily DevLog generated: {args.out}")
    print(f"   Commits: {gstats['count']}, +{gstats['added']}/-{gstats['deleted']}")
    print(f"   Metrics saved: {metrics_path}")

if __name__ == "__main__":
    main()
