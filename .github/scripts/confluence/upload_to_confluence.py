"""
GitHub Actions Confluence Upload Script
Uploads markdown files to Confluence automatically
"""
import os
import sys
import yaml
from pathlib import Path

# Add current directory to Python path for imports
sys.path.insert(0, os.path.dirname(__file__))

from converter import convert_md_to_html, read_markdown_file
from api import ConfluenceAPI
from utils import parse_filename_to_title, get_parent_id_from_path, validate_file_path


def load_config():
    """
    Load configuration from .github/config.yml and environment variables.

    Returns:
        Dictionary containing configuration

    Raises:
        FileNotFoundError: If config.yml is not found
        ValueError: If required configuration is missing
    """
    # Load config.yml
    config_path = Path(__file__).parent.parent.parent / 'config.yml'

    if not config_path.exists():
        raise FileNotFoundError(f"config.yml not found at: {config_path}")

    with open(config_path, 'r', encoding='utf-8') as f:
        config_data = yaml.safe_load(f)

    confluence_config = config_data.get('confluence', {})

    # Get API token from environment (GitHub Secrets)
    api_token = os.getenv('JIRA_API_TOKEN')
    if not api_token:
        raise ValueError("JIRA_API_TOKEN environment variable is required")

    return {
        'domain': confluence_config.get('domain'),
        'email': confluence_config.get('email'),
        'space_key': confluence_config.get('space_key'),
        'api_token': api_token,
        'parent_pages': confluence_config.get('parent_pages', {}),
        'enabled': confluence_config.get('enabled', True)
    }


def main():
    """
    Main function for uploading markdown to Confluence
    """
    # Get file path from command line argument
    if len(sys.argv) < 2:
        print("Usage: python upload_to_confluence.py <markdown_file_path>")
        sys.exit(1)

    file_path = sys.argv[1]

    try:
        # Load configuration
        print("📋 Loading configuration...")
        config = load_config()

        # Check if Confluence upload is enabled
        if not config['enabled']:
            print("ℹ️  Confluence upload is disabled in config.yml")
            sys.exit(0)

        # Validate file path
        print(f"🔍 Validating file: {file_path}")
        validate_file_path(file_path)

        # Get parent ID from file path
        print("🗂️  Determining parent page...")
        parent_id = get_parent_id_from_path(file_path, config['parent_pages'])
        print(f"   Parent page ID: {parent_id}")

        # Read markdown file
        print("📖 Reading markdown file...")
        md_content = read_markdown_file(file_path)

        # Convert to HTML
        print("🔄 Converting markdown to HTML...")
        html_content = convert_md_to_html(md_content)

        # Generate title from filename
        title = parse_filename_to_title(file_path)
        print(f"📝 Page title: {title}")

        # Initialize Confluence API
        print("🔌 Connecting to Confluence...")
        api = ConfluenceAPI(
            domain=config['domain'],
            email=config['email'],
            api_token=config['api_token'],
            space_key=config['space_key']
        )

        # Check if page already exists with same title
        print("🔎 Checking if page already exists...")
        existing_page_id = api.search_page_by_title(title, parent_id)

        if existing_page_id:
            # Update existing page
            print(f"♻️  Updating existing page (ID: {existing_page_id})...")
            page_info = api.get_page(existing_page_id)
            current_version = page_info['version']['number']

            result = api.update_page(
                page_id=existing_page_id,
                title=title,
                html_body=html_content,
                current_version=current_version
            )

            print("\n✅ Page updated successfully!")
            print(f"   Title: {result['title']}")
            print(f"   Page ID: {result['id']}")
            print(f"   Version: {result['version']['number']}")
            print(f"   URL: https://{config['domain']}/wiki/spaces/{config['space_key']}/pages/{result['id']}")

        else:
            # Create new page
            print("📄 Creating new page...")
            result = api.create_page(
                title=title,
                html_body=html_content,
                parent_id=parent_id
            )

            print("\n✅ Page created successfully!")
            print(f"   Title: {result['title']}")
            print(f"   Page ID: {result['id']}")
            print(f"   URL: https://{config['domain']}/wiki/spaces/{config['space_key']}/pages/{result['id']}")

    except FileNotFoundError as e:
        print(f"\n❌ Error: {str(e)}", file=sys.stderr)
        sys.exit(1)

    except ValueError as e:
        print(f"\n❌ Error: {str(e)}", file=sys.stderr)
        sys.exit(1)

    except Exception as e:
        print(f"\n❌ Unexpected error: {str(e)}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == '__main__':
    main()
