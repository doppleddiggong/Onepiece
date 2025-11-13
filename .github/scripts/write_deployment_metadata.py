"""Write deployment metadata consumed by downstream notification workflows."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timedelta, timezone
from pathlib import Path


KST = timezone(timedelta(hours=9))


def _build_metadata(event_name: str, trigger: str, latest_file: str) -> dict[str, str]:
    """Compose the metadata document describing the HonKit deployment."""

    return {
        "event_name": event_name,
        "trigger": trigger,
        "latest_file": latest_file,
        "generated_at": datetime.now(tz=timezone.utc).astimezone(KST).isoformat(),
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True, help="Destination metadata file path")
    parser.add_argument("--event", default="", help="GitHub event name that triggered the run")
    parser.add_argument("--trigger", default="", help="Client payload trigger identifier")
    parser.add_argument("--latest-file", default="", help="Most recent document path")
    args = parser.parse_args()

    metadata = _build_metadata(args.event, args.trigger, args.latest_file)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(metadata, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"🗂️ Deployment metadata saved to {args.output}")


if __name__ == "__main__":
    main()
