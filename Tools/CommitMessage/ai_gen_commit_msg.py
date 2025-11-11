#!/usr/bin/env python3
import os
import subprocess
import sys

from dotenv import load_dotenv
from openai import OpenAI

print("RUNNING FILE:", __file__)
print("PYTHON EXECUTABLE:", sys.executable)
print("CWD:", os.getcwd())

# ------------------------------------------------------------
# 초기 설정: .env 로드 + OpenAI 클라이언트 준비
# ------------------------------------------------------------
base_dir = os.path.dirname(os.path.abspath(__file__))
env_path = os.path.join(base_dir, ".env")
load_dotenv(env_path)

api_key = os.getenv("OPENAI_API_KEY")
if not api_key:
    print("OPENAI_API_KEY가 설정되지 않았습니다. .env 파일을 확인하세요.")
    sys.exit(1)

client = OpenAI(api_key=api_key)

# ------------------------------------------------------------
# Git diff
# ------------------------------------------------------------
def get_staged_diff(root):
    base_dir = os.path.dirname(os.path.abspath(__file__))

    cp = subprocess.run(
        ["git", "diff", "--cached"],
        cwd=root,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        encoding="utf-8",
    )
    diff = cp.stdout
    if not diff.strip():
        print("Staged 파일이 없습니다. `git add` 후 다시 실행하세요.")
        sys.exit(0)
    return diff

def repo_root(start_dir):
    cp = subprocess.run(
        ["git", "rev-parse", "--show-toplevel"],
        cwd=start_dir,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        encoding="utf-8",
    )
    return cp.stdout.strip() or start_dir

# ------------------------------------------------------------
# OpenAI API 요청
# ------------------------------------------------------------
def load_prompt(path):
    with open(path, "r", encoding="utf-8") as f:
        return f.read()
        

def ask_openai(diff_text):

    system_prompt_path = os.path.join(base_dir, "prompts/system_prompt.txt")
    user_prompt_path   = os.path.join(base_dir, "prompts/user_prompt.txt")

    system_prompt = load_prompt(system_prompt_path)
    user_prompt_raw = load_prompt(user_prompt_path)

    # {{DIFF}} 치환
    user_prompt = user_prompt_raw.replace("{{DIFF}}", diff_text)
    
    print("===== SYSTEM PROMPT =====".encode("utf-8", "ignore").decode("cp949", "ignore"))
    print(system_prompt.encode("utf-8", "ignore").decode("cp949", "ignore"))
    print("===== USER PROMPT =====".encode("utf-8", "ignore").decode("cp949", "ignore"))
    print(user_prompt.encode("utf-8", "ignore").decode("cp949", "ignore"))

    res = client.chat.completions.create(
        model="gpt-4.1-mini",
        messages=[
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": user_prompt}
        ],
        temperature=0.2,
        max_tokens=400
    )
    return res.choices[0].message.content.strip()

# ------------------------------------------------------------
# 파일 저장 (UTF-8 BOM)
# ------------------------------------------------------------
def save_message(content):
    msg_path = os.path.join(base_dir, "commit_message.txt")
    with open(msg_path, "w", encoding="utf-8-sig") as f:
        f.write(content)

# ------------------------------------------------------------
# 메인
# ------------------------------------------------------------
def main():
    start = os.getcwd()
    root = repo_root(start)

    diff_text = get_staged_diff(root)
    print("OpenAI 요청 시작")

    commit_message = ask_openai(diff_text)
    save_message(commit_message)

    print("커밋 메시지 생성 완료")
    print("-------------------------------------")
    print(commit_message)
    print("-------------------------------------")

if __name__ == "__main__":
    main()
