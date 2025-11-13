"""Prepare meeting notification metadata for Discord fan-out."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
from typing import Iterable, List, Tuple


def _parse_lines(raw: str | None) -> List[str]:
    """Return a sanitized list from a newline-delimited string."""

    if not raw:
        return []

    return [line.strip() for line in raw.splitlines() if line.strip()]


def _load_event_files(event_path: Path) -> Tuple[List[str], str]:
    """Extract file list and trigger identifier from the GitHub event payload."""

    event = json.loads(event_path.read_text(encoding="utf-8"))

    client_payload = event.get("client_payload") or {}
    trigger = client_payload.get("trigger", "meeting-update")

    files: List[str] = []
    files.extend(_parse_lines(client_payload.get("files")))

    manual_inputs = event.get("inputs") or {}
    files.extend(_parse_lines(manual_inputs.get("file_path")))

    deduped = list(dict.fromkeys(files))

    return deduped, trigger


def _write_outputs(files: Iterable[str], trigger: str, output_path: Path) -> None:
    """Write composite outputs for downstream workflow steps."""

    entries = list(files)
    with output_path.open("a", encoding="utf-8") as handle:
        if not entries:
            handle.write("has_files=false\n")
            handle.write(f"trigger={trigger}\n")
            return

        handle.write("has_files=true\n")
        handle.write(f"trigger={trigger}\n")
        handle.write(f"count={len(entries)}\n")


def _write_file_manifest(files: Iterable[str], manifest: Path) -> None:
    """Persist the meeting file manifest for notification fan-out."""

    entries = list(files)
    if not entries:
        if manifest.exists():
            manifest.unlink()
        return

    manifest.write_text("\n".join(entries), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--event-path", type=Path, required=True, help="Path to GitHub event payload")
    parser.add_argument(
        "--manifest",
        type=Path,
        required=True,
        help="Destination path for the newline-delimited meeting list",
    )
    args = parser.parse_args()

    files, trigger = _load_event_files(args.event_path)

    output_env = Path(os.environ.get("GITHUB_OUTPUT", ""))
    if not output_env:
        raise RuntimeError("GITHUB_OUTPUT environment variable is required")

    _write_file_manifest(files, args.manifest)
    _write_outputs(files, trigger, output_env)


if __name__ == "__main__":
    main()
