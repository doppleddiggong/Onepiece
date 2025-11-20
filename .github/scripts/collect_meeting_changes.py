"""Utility to prepare repository dispatch payloads for meeting updates."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
from typing import Iterable, List

MEETING_PREFIX = "Documents/Meeting/"

def _load_added_meetings(diff_file: Path) -> List[str]:
    """Return meeting files that were newly added in *diff_file*."""
    
    if not diff_file.exists():
        return []

    added: List[str] = []
    for raw_line in diff_file.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line:
            continue

        parts = line.split(maxsplit=1)
        if len(parts) != 2:
            continue

        status, file_path = parts
        normalized = Path(file_path).as_posix()
        if status == "A" and normalized.startswith(MEETING_PREFIX):
            added.append(normalized)

    return sorted(added)


def _write_outputs(lines: Iterable[str], target_ref: str, output_path: Path) -> None:
    """Append GitHub Actions outputs for the detected file list."""

    def _write_output(handle, name: str, value: str) -> None:
        text = str(value)
        if "\n" in text:
            delimiter = "EOF"
            while delimiter in text:
                delimiter = f"EOF_{len(delimiter)}"
            handle.write(f"{name}<<{delimiter}\n{text}\n{delimiter}\n")
        else:
            handle.write(f"{name}={text}\n")

    files = list(lines)
    with output_path.open("a", encoding="utf-8") as handle:
        if not files:
            handle.write("has_changes=false\n")
            return

        payload = {
            "ref": target_ref,
            "trigger": "meeting-update",
            "latest_file": files[0],
            "files": "\n".join(files),
        }

        broadcast_payload = {
            "trigger": "meeting-update",
            "files": "\n".join(files),
        }

        confluence_payload = {
            "files": files,
        }

        handle.write("has_changes=true\n")
        _write_output(handle, "honkit_payload", json.dumps(payload, ensure_ascii=False))
        _write_output(handle, "broadcast_payload", json.dumps(broadcast_payload, ensure_ascii=False))
        _write_output(handle, "confluence_payload", json.dumps(confluence_payload, ensure_ascii=False))
        handle.write(f"file_count={len(files)}\n")


def _write_file_list(files: Iterable[str], destination: Path) -> None:
    """Persist the detected file list to *destination* if there are entries."""

    file_list = list(files)
    if not file_list:
        if destination.exists():
            destination.unlink()
        return

    destination.write_text("\n".join(file_list), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--diff-file", type=Path, required=True, help="Path to git diff output")
    parser.add_argument(
        "--list-output",
        type=Path,
        required=True,
        help="File path to write the sanitized meeting list",
    )
    parser.add_argument(
        "--target-ref",
        default=os.environ.get("TARGET_REF", "main"),
        help="Reference name associated with the push",
    )
    args = parser.parse_args()

    files = _load_added_meetings(args.diff_file)

    github_output = os.environ.get("GITHUB_OUTPUT")
    if not github_output:
        raise RuntimeError("GITHUB_OUTPUT environment variable is required")

    _write_file_list(files, args.list_output)
    _write_outputs(files, args.target_ref, Path(github_output))


if __name__ == "__main__":
    main()
