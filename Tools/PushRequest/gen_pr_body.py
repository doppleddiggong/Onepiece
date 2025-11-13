# -*- coding: utf-8 -*-
import sys
sys.stdout.reconfigure(encoding='utf-8')

#!/usr/bin/env python3
import argparse
from pathlib import Path

def categorize(files):
    cats = {
        "Workflows": [],
        "Python Scripts": [],
        "DevLog": [],
        "Meeting": [],
        "HonKit": [],
        "Docs": [],
        "Other": [],
    }

    for f in files:
        p = Path(f)
        s = str(p)

        if s.startswith(".github/workflows"):
            cats["Workflows"].append(s)
        elif s.startswith(".github/scripts"):
            cats["Python Scripts"].append(s)
        elif s.startswith("Documents/DevLog"):
            cats["DevLog"].append(s)
        elif s.startswith("Documents/Meeting"):
            cats["Meeting"].append(s)
        elif "honkit" in s.lower():
            cats["HonKit"].append(s)
        elif s.endswith(".md"):
            cats["Docs"].append(s)
        else:
            cats["Other"].append(s)
    return cats

def render(cats, files):
    body = []

    # 요약
    body.append("## 요약\n")
    body.append(f"- 총 {len(files)}개의 파일이 변경되었습니다.\n")

    # 변경 내용
    body.append("\n## 변경 내용\n")
    for c, fl in cats.items():
        if fl:
            body.append(f"### {c}\n")
            for f in fl:
                body.append(f"- {f}\n")

    # 영향 범위
    body.append("\n## 영향 범위\n")
    for c, fl in cats.items():
        if fl:
            body.append(f"- {c}: {len(fl)}개 파일\n")

    # 테스트
    body.append("\n## 테스트\n")
    body.append("- DevLog/HonKit/Meeting/Workflow 관련 영향 예상\n")

    # 리뷰 요청
    body.append("\n## 리뷰 요청\n")
    body.append("@codex review\n")
    body.append("Please respond in Korean.\n")

    return "\n".join(body)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--files", type=str, required=True)
    args = parser.parse_args()

    files = [f.strip() for f in args.files.splitlines() if f.strip()]

    cats = categorize(files)
    print(render(cats, files))

if __name__ == "__main__":
    main()
