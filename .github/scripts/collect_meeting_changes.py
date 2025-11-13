"""Utility to prepare repository dispatch payloads for meeting updates."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
from typing import Iterable, List


def _load_changed_files(diff_file: Path) -> List[str]:
    """Return sanitized file paths listed in *diff_file*.

    Args:
        diff_file: Path to the text file produced by ``git diff``.

    Returns:
        A list of non-empty, stripped file paths.
    """

    if not diff_file.exists():
        return []

    lines = diff_file.read_text(encoding="utf-8").splitlines()
    return [line.strip() for line in lines if line.strip()]


def _write_outputs(lines: Iterable[str], target_ref: str, output_path: Path) -> None:
    """Append GitHub Actions outputs for the detected file list."""

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

        handle.write("has_changes=true\n")
        handle.write(f"honkit_payload={json.dumps(payload, ensure_ascii=False)}\n")
        handle.write(f"broadcast_payload={json.dumps(broadcast_payload, ensure_ascii=False)}\n")
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

    files = _load_changed_files(args.diff_file)

    github_output = os.environ.get("GITHUB_OUTPUT")
    if not github_output:
        raise RuntimeError("GITHUB_OUTPUT environment variable is required")

    _write_file_list(files, args.list_output)
    _write_outputs(files, args.target_ref, Path(github_output))


if __name__ == "__main__":
    main()
