#!/usr/bin/env python3
"""
캐시 관리자
GPT API 응답을 파일 기반으로 캐싱하여 중복 요청 방지
"""

import hashlib
import json
import os
from datetime import datetime, timedelta
from pathlib import Path


class CacheManager:
    """GPT API 응답 캐시 관리자"""

    def __init__(self, cache_dir=None, ttl_hours=24):
        """
        캐시 관리자 초기화

        Args:
            cache_dir: 캐시 디렉토리 경로 (None이면 스크립트 폴더/.cache 사용)
            ttl_hours: 캐시 유효 시간 (시간 단위)
        """
        if cache_dir is None:
            script_dir = Path(__file__).parent
            cache_dir = script_dir / '.cache'

        self.cache_dir = Path(cache_dir)
        self.ttl_hours = ttl_hours

        # 캐시 디렉토리 생성
        self.cache_dir.mkdir(parents=True, exist_ok=True)

    def _generate_cache_key(self, data):
        """
        데이터에서 캐시 키 생성 (SHA256 해시)

        Args:
            data: 캐시할 데이터 (딕셔너리)

        Returns:
            str: 캐시 키 (SHA256 해시)
        """
        # 캐시 키 생성에 사용할 주요 데이터만 추출
        cache_data = {
            'date': data.get('date'),
            'commits': [c['subject'] for c in data.get('commits', [])],
            'changed_files': data.get('changed_files', []),
            'diff_hash': hashlib.sha256(data.get('diff', '').encode()).hexdigest()[:16],
            'meeting_title': data.get('meeting', {}).get('title') if data.get('meeting') else None
        }

        # JSON 문자열로 변환 후 해시 생성
        cache_str = json.dumps(cache_data, sort_keys=True, ensure_ascii=False)
        cache_hash = hashlib.sha256(cache_str.encode()).hexdigest()

        return cache_hash

    def _get_cache_path(self, cache_key):
        """
        캐시 파일 경로 생성

        Args:
            cache_key: 캐시 키

        Returns:
            Path: 캐시 파일 경로
        """
        return self.cache_dir / f"{cache_key}.json"

    def _is_cache_valid(self, cache_path):
        """
        캐시가 유효한지 확인 (TTL 체크)

        Args:
            cache_path: 캐시 파일 경로

        Returns:
            bool: 캐시 유효 여부
        """
        if not cache_path.exists():
            return False

        # 파일 수정 시간 확인
        mtime = datetime.fromtimestamp(cache_path.stat().st_mtime)
        now = datetime.now()

        # TTL 체크
        if now - mtime > timedelta(hours=self.ttl_hours):
            return False

        return True

    def get(self, data):
        """
        캐시에서 데이터 조회

        Args:
            data: 조회할 데이터

        Returns:
            str or None: 캐시된 DevLog 내용 (없으면 None)
        """
        cache_key = self._generate_cache_key(data)
        cache_path = self._get_cache_path(cache_key)

        # 캐시 유효성 확인
        if not self._is_cache_valid(cache_path):
            return None

        try:
            # 캐시 파일 읽기
            with open(cache_path, 'r', encoding='utf-8') as f:
                cache_data = json.load(f)

            print(f"  [CACHE] 캐시 히트! (키: {cache_key[:16]}...)")
            return cache_data.get('devlog_content')

        except Exception as e:
            print(f"  [WARN]  캐시 읽기 실패: {e}")
            return None

    def set(self, data, devlog_content):
        """
        캐시에 데이터 저장

        Args:
            data: 저장할 데이터
            devlog_content: DevLog 내용
        """
        cache_key = self._generate_cache_key(data)
        cache_path = self._get_cache_path(cache_key)

        try:
            # 캐시 데이터 생성
            cache_data = {
                'cache_key': cache_key,
                'cached_at': datetime.now().isoformat(),
                'ttl_hours': self.ttl_hours,
                'devlog_content': devlog_content,
                'metadata': {
                    'date': data.get('date'),
                    'developer': data.get('developer'),
                    'commit_count': len(data.get('commits', []))
                }
            }

            # 캐시 파일 저장
            with open(cache_path, 'w', encoding='utf-8') as f:
                json.dump(cache_data, f, ensure_ascii=False, indent=2)

            print(f"  [CACHE] 캐시 저장 완료 (키: {cache_key[:16]}...)")

        except Exception as e:
            print(f"  [WARN]  캐시 저장 실패: {e}")

    def clear(self, older_than_hours=None):
        """
        캐시 정리

        Args:
            older_than_hours: 지정한 시간보다 오래된 캐시만 삭제 (None이면 전체 삭제)

        Returns:
            int: 삭제된 캐시 파일 수
        """
        deleted_count = 0

        for cache_file in self.cache_dir.glob('*.json'):
            try:
                if older_than_hours is None:
                    # 전체 삭제
                    cache_file.unlink()
                    deleted_count += 1
                else:
                    # TTL 기반 삭제
                    mtime = datetime.fromtimestamp(cache_file.stat().st_mtime)
                    now = datetime.now()

                    if now - mtime > timedelta(hours=older_than_hours):
                        cache_file.unlink()
                        deleted_count += 1

            except Exception as e:
                print(f"  [WARN]  캐시 파일 삭제 실패 ({cache_file.name}): {e}")

        print(f"  [CACHE] {deleted_count}개 캐시 파일 삭제 완료")
        return deleted_count

    def get_stats(self):
        """
        캐시 통계 조회

        Returns:
            dict: 캐시 통계 정보
        """
        cache_files = list(self.cache_dir.glob('*.json'))
        total_size = sum(f.stat().st_size for f in cache_files)

        valid_count = 0
        expired_count = 0

        for cache_file in cache_files:
            if self._is_cache_valid(cache_file):
                valid_count += 1
            else:
                expired_count += 1

        return {
            'total_count': len(cache_files),
            'valid_count': valid_count,
            'expired_count': expired_count,
            'total_size_mb': total_size / (1024 * 1024),
            'cache_dir': str(self.cache_dir)
        }


if __name__ == "__main__":
    # 테스트 코드
    print("=== 캐시 관리자 테스트 ===\n")

    # 캐시 관리자 초기화
    cache_mgr = CacheManager(ttl_hours=24)

    # 테스트 데이터
    test_data = {
        "date": "2025-11-25",
        "developer": "배주백",
        "commits": [
            {"short_hash": "abc123", "subject": "feat: add cache system"},
            {"short_hash": "def456", "subject": "fix: resolve memory leak"}
        ],
        "changed_files": ["gpt_client.py", "cache_manager.py"],
        "diff": "diff --git a/gpt_client.py...",
        "meeting": {"title": "Daily Meeting"}
    }

    # 캐시 저장 테스트
    print("1. 캐시 저장 테스트")
    cache_mgr.set(test_data, "테스트 DevLog 내용")

    # 캐시 조회 테스트
    print("\n2. 캐시 조회 테스트")
    cached_content = cache_mgr.get(test_data)
    if cached_content:
        print(f"  [OK] 캐시 조회 성공: {cached_content}")
    else:
        print(f"  [FAIL] 캐시 조회 실패")

    # 캐시 통계 테스트
    print("\n3. 캐시 통계 테스트")
    stats = cache_mgr.get_stats()
    print(f"  총 캐시 파일: {stats['total_count']}개")
    print(f"  유효 캐시: {stats['valid_count']}개")
    print(f"  만료 캐시: {stats['expired_count']}개")
    print(f"  총 크기: {stats['total_size_mb']:.2f} MB")
    print(f"  캐시 디렉토리: {stats['cache_dir']}")

    # 캐시 정리 테스트
    print("\n4. 캐시 정리 테스트")
    deleted = cache_mgr.clear(older_than_hours=0)  # 즉시 삭제 (테스트용)
