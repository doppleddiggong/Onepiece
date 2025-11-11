#!/usr/bin/env bash
set -e
root="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
cd "$root"

py="$root/Tools/CommitMessage/venv/bin/python"
if [ ! -x "$py" ]; then py="$root/Tools/CommitMessage/venv/bin/python3"; fi

if [ -f "$root/Tools/CommitMessage/ai_gen_commit_msg.py" ]; then
  "$py" "$root/Tools/CommitMessage/ai_gen_commit_msg.py"
else
  "$py" "$root/Tools/CommitMessage/gen_commit_msg.py" -MsgPath "$root/Tools/CommitMessage/commit_message.txt" -ToClipboard
fi
status=$?
if [ $status -eq 0 ]; then
  echo "커밋 메시지 생성 완료 (commit_message.txt)"
else
  echo "커밋 메시지 생성 실패 (exit $status)"
fi
exit $status
