"""
Utility Functions Module
"""
import os
import re
from pathlib import Path


def parse_filename_to_title(file_path: str) -> str:
    """
    Parse filename to generate a title automatically.

    Example:
        Meeting_251117_Daily.md -> 2025-11-17 — Daily Log
        251117_meeting.md -> 2025-11-17 — meeting

    Args:
        file_path: Path to the markdown file

    Returns:
        Generated title string
    """
    # Extract filename without extension
    filename = os.path.basename(file_path)
    name_without_ext = os.path.splitext(filename)[0]

    # Try to extract date pattern (YYMMDD)
    date_pattern = r'(\d{6})'
    date_match = re.search(date_pattern, name_without_ext)

    if date_match:
        date_str = date_match.group(1)
        try:
            # Convert YYMMDD to YYYY-MM-DD
            year = int('20' + date_str[:2])
            month = int(date_str[2:4])
            day = int(date_str[4:6])
            formatted_date = f"{year}-{month:02d}-{day:02d}"

            # Extract remaining parts (like "Daily", "Common", etc.)
            remaining = re.sub(r'[\d_]+', ' ', name_without_ext).strip()
            remaining = re.sub(r'\s+', ' ', remaining)

            if remaining:
                return f"{formatted_date} — {remaining}"
            else:
                return f"{formatted_date} — Log"
        except ValueError:
            pass

    # If date pattern not found, use filename as title
    # Replace underscores with spaces and clean up
    title = name_without_ext.replace('_', ' ')
    title = re.sub(r'\s+', ' ', title).strip()
    return title


def get_parent_id_from_path(file_path: str, parent_pages: dict) -> str:
    """
    Get parent page ID based on file path.

    Mapping:
        Documents/Meeting/Daily -> parent_pages['daily']
        Documents/Meeting/Common -> parent_pages['common']
        Documents/DevLog/Daily -> parent_pages['ue']
        Documents/DevLog/Weekly -> parent_pages['ue']

    Args:
        file_path: Path to the markdown file
        parent_pages: Dictionary mapping page types to parent IDs

    Returns:
        Parent page ID as string

    Raises:
        ValueError: If file path doesn't match any known patterns
    """
    # Normalize path separators
    normalized_path = file_path.replace('\\', '/')

    # Check path patterns
    if 'Documents/Meeting/Daily' in normalized_path:
        return parent_pages.get('daily', '')
    elif 'Documents/Meeting/Common' in normalized_path:
        return parent_pages.get('common', '')
    elif 'Documents/DevLog/Daily' in normalized_path:
        return parent_pages.get('ue', '')
    elif 'Documents/DevLog/Weekly' in normalized_path:
        return parent_pages.get('ue', '')
    else:
        raise ValueError(
            f"File path '{file_path}' doesn't match any known Confluence upload patterns.\n"
            "Expected paths:\n"
            "  - Documents/Meeting/Daily/*.md\n"
            "  - Documents/Meeting/Common/*.md\n"
            "  - Documents/DevLog/Daily/*.md\n"
            "  - Documents/DevLog/Weekly/*.md"
        )


def validate_file_path(file_path: str) -> bool:
    """
    Validate if the file path exists and is a markdown file.

    Args:
        file_path: Path to validate

    Returns:
        True if valid, False otherwise

    Raises:
        FileNotFoundError: If file doesn't exist
        ValueError: If file is not a markdown file
    """
    if not os.path.exists(file_path):
        raise FileNotFoundError(f"File not found: {file_path}")

    if not file_path.lower().endswith('.md'):
        raise ValueError(f"File must be a markdown (.md) file: {file_path}")

    return True
