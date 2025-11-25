#!/usr/bin/env python3
"""
캐시 관리 유틸리티
GPT API 응답 캐시를 관리하는 CLI 도구
"""

import argparse
import sys
from pathlib import Path

# 현재 스크립트 디렉토리를 모듈 경로에 추가
script_dir = Path(__file__).parent
sys.path.insert(0, str(script_dir))

from cache_manager import CacheManager


def show_stats(cache_mgr):
    """캐시 통계 출력"""
    print("\n=== 캐시 통계 ===")
    stats = cache_mgr.get_stats()

    print(f"총 캐시 파일: {stats['total_count']}개")
    print(f"유효 캐시: {stats['valid_count']}개")
    print(f"만료 캐시: {stats['expired_count']}개")
    print(f"총 크기: {stats['total_size_mb']:.2f} MB")
    print(f"캐시 디렉토리: {stats['cache_dir']}")

    if stats['total_count'] > 0:
        hit_rate = (stats['valid_count'] / stats['total_count']) * 100
        print(f"유효율: {hit_rate:.1f}%")


def clear_cache(cache_mgr, older_than_hours=None, force=False):
    """캐시 정리"""
    if older_than_hours:
        print(f"\n=== {older_than_hours}시간 이상 된 캐시 정리 ===")
    else:
        print("\n=== 전체 캐시 정리 ===")

    # 확인
    if not force:
        confirm = input("정말로 캐시를 삭제하시겠습니까? (y/N): ")
        if confirm.lower() != 'y':
            print("취소되었습니다.")
            return

    deleted_count = cache_mgr.clear(older_than_hours=older_than_hours)
    print(f"\n✅ {deleted_count}개 캐시 파일 삭제 완료")


def main():
    """메인 함수"""
    parser = argparse.ArgumentParser(
        description='KLingo DevLog 캐시 관리 유틸리티',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
사용 예시:
  # 캐시 통계 보기
  python cache_util.py stats

  # 24시간 이상 된 캐시 삭제
  python cache_util.py clear --older-than 24

  # 전체 캐시 삭제
  python cache_util.py clear --all

  # 강제 삭제 (확인 없이)
  python cache_util.py clear --all --force
        """
    )

    parser.add_argument(
        'command',
        choices=['stats', 'clear'],
        help='실행할 명령'
    )

    parser.add_argument(
        '--older-than',
        type=int,
        metavar='HOURS',
        help='지정한 시간(시간 단위)보다 오래된 캐시만 삭제'
    )

    parser.add_argument(
        '--all',
        action='store_true',
        help='전체 캐시 삭제'
    )

    parser.add_argument(
        '--force',
        action='store_true',
        help='확인 없이 강제 삭제'
    )

    parser.add_argument(
        '--cache-dir',
        type=str,
        help='캐시 디렉토리 경로 (기본: .cache)'
    )

    args = parser.parse_args()

    # 캐시 관리자 초기화
    cache_mgr = CacheManager(cache_dir=args.cache_dir if args.cache_dir else None)

    # 명령 실행
    if args.command == 'stats':
        show_stats(cache_mgr)

    elif args.command == 'clear':
        if args.all:
            clear_cache(cache_mgr, older_than_hours=None, force=args.force)
        elif args.older_than:
            clear_cache(cache_mgr, older_than_hours=args.older_than, force=args.force)
        else:
            print("❌ 오류: --all 또는 --older-than 옵션을 지정해야 합니다.")
            parser.print_help()
            sys.exit(1)


if __name__ == "__main__":
    main()
