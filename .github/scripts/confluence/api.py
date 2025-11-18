"""
Confluence REST API Module
"""
import requests
from typing import Dict, Optional


class ConfluenceAPI:
    """
    Confluence REST API wrapper class
    """

    def __init__(self, domain: str, email: str, api_token: str, space_key: str):
        """
        Initialize Confluence API client.

        Args:
            domain: Confluence domain (e.g., your-domain.atlassian.net)
            email: User email for authentication
            api_token: API token for authentication
            space_key: Confluence space key
        """
        self.domain = domain
        self.email = email
        self.api_token = api_token
        self.space_key = space_key
        self.base_url = f"https://{domain}/wiki/rest/api"
        self.auth = (email, api_token)

    def create_page(self, title: str, html_body: str, parent_id: str) -> Dict:
        """
        Create a new Confluence page.

        Args:
            title: Page title
            html_body: HTML content for the page body
            parent_id: Parent page ID

        Returns:
            Response data as dictionary

        Raises:
            requests.exceptions.RequestException: If the API request fails
        """
        url = f"{self.base_url}/content"

        payload = {
            "type": "page",
            "title": title,
            "space": {"key": self.space_key},
            "ancestors": [{"id": parent_id}],
            "body": {
                "storage": {
                    "value": html_body,
                    "representation": "storage"
                }
            }
        }

        try:
            response = requests.post(
                url,
                json=payload,
                auth=self.auth,
                headers={"Content-Type": "application/json"}
            )
            response.raise_for_status()
            return response.json()
        except requests.exceptions.HTTPError as e:
            error_msg = f"HTTP error occurred: {e}"
            if response.text:
                error_msg += f"\nResponse: {response.text}"
            raise requests.exceptions.RequestException(error_msg)
        except requests.exceptions.RequestException as e:
            raise requests.exceptions.RequestException(f"API request failed: {str(e)}")

    def get_page(self, page_id: str) -> Dict:
        """
        Get page information including version.

        Args:
            page_id: Page ID to retrieve

        Returns:
            Response data as dictionary

        Raises:
            requests.exceptions.RequestException: If the API request fails
        """
        url = f"{self.base_url}/content/{page_id}?expand=version"

        try:
            response = requests.get(url, auth=self.auth)
            response.raise_for_status()
            return response.json()
        except requests.exceptions.HTTPError as e:
            error_msg = f"HTTP error occurred: {e}"
            if response.text:
                error_msg += f"\nResponse: {response.text}"
            raise requests.exceptions.RequestException(error_msg)
        except requests.exceptions.RequestException as e:
            raise requests.exceptions.RequestException(f"API request failed: {str(e)}")

    def update_page(self, page_id: str, title: str, html_body: str, current_version: int) -> Dict:
        """
        Update an existing Confluence page.

        Args:
            page_id: Page ID to update
            title: New page title
            html_body: New HTML content for the page body
            current_version: Current version number of the page

        Returns:
            Response data as dictionary

        Raises:
            requests.exceptions.RequestException: If the API request fails
        """
        url = f"{self.base_url}/content/{page_id}"

        payload = {
            "type": "page",
            "title": title,
            "version": {"number": current_version + 1},
            "body": {
                "storage": {
                    "value": html_body,
                    "representation": "storage"
                }
            }
        }

        try:
            response = requests.put(
                url,
                json=payload,
                auth=self.auth,
                headers={"Content-Type": "application/json"}
            )
            response.raise_for_status()
            return response.json()
        except requests.exceptions.HTTPError as e:
            error_msg = f"HTTP error occurred: {e}"
            if response.text:
                error_msg += f"\nResponse: {response.text}"
            raise requests.exceptions.RequestException(error_msg)
        except requests.exceptions.RequestException as e:
            raise requests.exceptions.RequestException(f"API request failed: {str(e)}")

    def search_page_by_title(self, title: str, parent_id: str) -> Optional[str]:
        """
        Search for a page by title under a specific parent.

        Args:
            title: Page title to search for
            parent_id: Parent page ID

        Returns:
            Page ID if found, None otherwise
        """
        url = f"{self.base_url}/content/search"
        params = {
            "cql": f"title=\"{title}\" and space={self.space_key} and ancestor={parent_id}",
            "limit": 1
        }

        try:
            response = requests.get(url, params=params, auth=self.auth)
            response.raise_for_status()
            results = response.json()

            if results.get('results'):
                return results['results'][0]['id']
            return None
        except Exception:
            return None
