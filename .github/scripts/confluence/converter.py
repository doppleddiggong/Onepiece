"""
Markdown to HTML Converter Module
"""
import markdown
import re


def remove_yaml_front_matter(md_content: str) -> str:
    """
    Remove YAML front matter from markdown content.

    Args:
        md_content: Markdown formatted string

    Returns:
        Markdown string without YAML front matter
    """
    # Pattern to match YAML front matter (--- at start, content, --- at end)
    pattern = r'^---\s*\n.*?\n---\s*\n'
    cleaned = re.sub(pattern, '', md_content, flags=re.DOTALL)
    return cleaned


def convert_md_to_html(md_content: str) -> str:
    """
    Convert Markdown content to HTML using markdown library.

    Args:
        md_content: Markdown formatted string

    Returns:
        HTML formatted string
    """
    # Remove YAML front matter before conversion
    md_content = remove_yaml_front_matter(md_content)

    # Use markdown with extensions for code blocks and tables
    html = markdown.markdown(
        md_content,
        extensions=['fenced_code', 'tables', 'nl2br']
    )
    return html


def read_markdown_file(file_path: str) -> str:
    """
    Read markdown file and return its content.

    Args:
        file_path: Path to the markdown file

    Returns:
        File content as string

    Raises:
        FileNotFoundError: If the file doesn't exist
    """
    try:
        with open(file_path, 'r', encoding='utf-8')  as f:
            return f.read()
    except FileNotFoundError:
        raise FileNotFoundError(f"Markdown file not found: {file_path}")
    except Exception as e:
        raise Exception(f"Error reading file {file_path}: {str(e)}")
