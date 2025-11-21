"""Send Discord notifications for updated meeting documents."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
from typing import Iterable
from urllib.error import HTTPError
from urllib.parse import quote
from urllib.request import Request, urlopen


def _load_files(manifest: Path) -> list[str]:
    """Read newline-delimited meeting file paths from *manifest*."""

    if not manifest.exists():
        return []

    return [line.strip() for line in manifest.read_text(encoding="utf-8").splitlines() if line.strip()]


def _to_html_path(meeting_path: str) -> str:
    """Convert a Markdown meeting path to its published HTML relative path."""

    relative = meeting_path
    if relative.startswith("Documents/"):
        relative = relative[len("Documents/") :]

    if relative.endswith(".md"):
        relative = relative[:-3] + ".html"

    return "/".join(quote(part) for part in relative.split("/"))


def _send_notification(webhook: str, content: str) -> None:
    body = json.dumps({"content": content}, ensure_ascii=False).encode("utf-8")
    headers = {
        "Content-Type": "application/json",
        "User-Agent": "GitHubActionsWebhook/1.0"
    }
    request = Request(webhook, data=body, headers=headers)
    with urlopen(request) as response:  # noqa: S310 - GitHub-hosted runner trusted URL
        response.read()


def _iter_messages(files: Iterable[str], base_url: str) -> Iterable[tuple[str, str]]:
    for path in files:
        title = path.split("/")[-1]
        html_path = _to_html_path(path)
        url = f"{base_url}/{html_path}"
        content = f"📘 회의록 **{title}** 이(가) 업데이트되었습니다!\n🔗 {url}"
        yield path, content


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, required=True, help="Path to meeting file manifest")
    parser.add_argument(
        "--base-url",
        default="https://doppleddiggong.github.io/Onepiece/honkit",
        help="Base URL for published HonKit documents",
    )
    args = parser.parse_args()

    webhook = (
        os.environ.get("DISCORD_WEBHOOK_MEETING")
        or os.environ.get("DISCORD_WEBHOOK_DEFAULT")
        or os.environ.get("DISCORD_WEBHOOK")
    )
    if not webhook:
        print("⚠️ Discord 웹훅이 설정되지 않았습니다. 알림을 생략합니다.")
        return

    files = _load_files(args.manifest)
    transient_errors: list[str] = []
    for path, message in _iter_messages(files, args.base_url):
        try:
            _send_notification(webhook, message)
            print(f"📨 Discord 알림 전송 완료: {path}")
        except HTTPError as exc:
            print(f"❌ Discord 전송 실패 ({path}): {exc.code} {exc.reason}")

            if exc.code >= 500 or exc.code == 429:
                transient_errors.append(path)
            else:
                print("⚠️ 권한 또는 설정 문제로 인해 알림을 건너뜁니다.")

    if transient_errors:
        raise SystemExit(
            "일시적인 오류로 인해 일부 알림이 전송되지 않았습니다: "
            + ", ".join(transient_errors)
        )


if __name__ == "__main__":
    main()
