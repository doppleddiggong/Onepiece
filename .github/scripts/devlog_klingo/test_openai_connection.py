#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
OpenAI API 연결 테스트 스크립트
GitHub Actions에서 실행하여 API 키와 연결 상태를 확인합니다.
"""

import os
import sys
import io

# Windows 콘솔 인코딩 문제 해결
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

def load_env_file():
    """config.env 파일에서 환경 변수 로드"""
    from pathlib import Path

    # 프로젝트 루트의 config.env 찾기
    script_dir = Path(__file__).parent
    env_paths = [
        script_dir.parent.parent / 'config.env',  # .github/config.env
        script_dir / 'config.env',  # devlog_klingo/config.env
    ]

    for env_path in env_paths:
        if env_path.exists():
            print(f"📁 Loading environment from: {env_path}")
            with open(env_path, 'r', encoding='utf-8') as f:
                for line in f:
                    line = line.strip()
                    if not line or line.startswith('#'):
                        continue
                    if '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        if key and not os.getenv(key):
                            os.environ[key] = value
            return True
    return False

def test_openai_connection():
    """OpenAI API 연결 및 키 유효성 테스트"""

    print("=" * 50)
    print("OpenAI API Connection Test")
    print("=" * 50)
    print()

    # 0. config.env 로드 시도
    load_env_file()

    # 1. API 키 존재 확인
    api_key = os.getenv('OPENAI_API_KEY', '')

    if not api_key:
        print("❌ OPENAI_API_KEY not found in environment")
        return False

    print(f"✅ API Key found (length: {len(api_key)})")
    print(f"   Prefix: {api_key[:10]}...")
    print(f"   Suffix: ...{api_key[-4:]}")

    # 2. OpenAI 라이브러리 임포트
    try:
        import openai
        print(f"✅ OpenAI library version: {openai.__version__}")
    except ImportError as e:
        print(f"❌ Failed to import openai: {e}")
        return False

    # 3. API 연결 테스트
    try:
        from openai import OpenAI
        client = OpenAI(api_key=api_key)

        print("\n🔄 Testing API connection...")

        # 간단한 API 호출 (최소 비용)
        response = client.chat.completions.create(
            model="gpt-4o-mini",  # 가장 저렴한 모델
            messages=[
                {"role": "user", "content": "Hello"}
            ],
            max_tokens=5
        )

        print(f"✅ API Connection successful!")
        print(f"   Model: {response.model}")
        print(f"   Response: {response.choices[0].message.content}")

        return True

    except Exception as e:
        print(f"❌ API Connection failed: {type(e).__name__}")
        print(f"   Error: {str(e)}")

        # 에러 타입별 상세 정보
        if "authentication" in str(e).lower():
            print("\n💡 Suggestion: Check if your API key is valid")
            print("   Visit: https://platform.openai.com/api-keys")
        elif "quota" in str(e).lower() or "billing" in str(e).lower():
            print("\n💡 Suggestion: Check your OpenAI billing/quota")
            print("   Visit: https://platform.openai.com/account/billing")
        elif "connection" in str(e).lower() or "timeout" in str(e).lower():
            print("\n💡 Suggestion: Network connectivity issue")
            print("   - Check if GitHub Actions can reach api.openai.com")
            print("   - Try again later (temporary network issue)")

        return False

if __name__ == "__main__":
    print()
    success = test_openai_connection()
    print()
    print("=" * 50)

    if success:
        print("✅ All tests passed!")
        sys.exit(0)
    else:
        print("❌ Test failed - see details above")
        sys.exit(1)
