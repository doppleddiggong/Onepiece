#!/usr/bin/env python3
"""
KLingo 주간 DevLog 자동 생성 시스템
Daily DevLog들을 통합하여 Weekly DevLog 생성
"""

import argparse
import json
import os
import sys
from pathlib import Path
from datetime import datetime, date

# 현재 스크립트 디렉토리를 모듈 경로에 추가
script_dir = Path(__file__).parent
sys.path.insert(0, str(script_dir))

# 로컬 모듈 임포트
import weekly_stats
import utils


def load_env_file(env_path):
    """
    config.env 파일에서 환경 변수 로드

    Args:
        env_path: .env 파일 경로
    """
    env_file = Path(env_path)
    if not env_file.exists():
        return

    with open(env_file, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            # 주석이나 빈 줄 무시
            if not line or line.startswith('#'):
                continue

            # KEY=VALUE 형식 파싱
            if '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()

                # 이미 환경 변수에 설정되어 있지 않으면 설정
                if key and not os.getenv(key):
                    os.environ[key] = value

    print(f"[*] Loaded environment variables from {env_path}")


def load_config(config_path):
    """
    설정 파일 로딩

    Args:
        config_path: config.json 경로

    Returns:
        dict: 설정 데이터
    """
    config_file = Path(config_path)

    if not config_file.exists():
        print(f"[WARN] 설정 파일을 찾을 수 없습니다: {config_path}")
        return {}

    with open(config_file, 'r', encoding='utf-8') as f:
        return json.load(f)


def generate_weekly_devlog_with_gpt(data, api_key, model='gpt-4o'):
    """
    GPT API를 사용하여 주간 DevLog 생성

    Args:
        data: 주간 데이터
        api_key: OpenAI API 키
        model: GPT 모델명

    Returns:
        str: 생성된 DevLog 본문
    """
    try:
        from openai import OpenAI
    except ImportError:
        print("[WARN] OpenAI 패키지가 설치되어 있지 않습니다")
        return None

    # 프롬프트 템플릿 로드
    prompt_template_path = script_dir / 'templates' / 'weekly_devlog_prompt.txt'

    if not prompt_template_path.exists():
        print(f"[WARN] 프롬프트 템플릿을 찾을 수 없습니다: {prompt_template_path}")
        return None

    prompt_template = prompt_template_path.read_text(encoding='utf-8')

    # 데이터 요약 생성
    stats = data.get('stats', {})
    daily_summaries = data.get('daily_summaries', [])

    # 일별 요약 포맷팅
    daily_summary_text = ""
    for summary in daily_summaries:
        daily_summary_text += f"\n**{summary['date']}**:\n{summary['summary']}\n"

    if not daily_summary_text:
        daily_summary_text = "이번 주 Daily DevLog 없음"

    # 시스템 요약 포맷팅
    top_systems = stats.get('top_systems', [])
    systems_text = ""
    for system, count in top_systems[:10]:
        systems_text += f"- {system}: {count}개 파일\n"

    if not systems_text:
        systems_text = "변경된 시스템 없음"

    # 프롬프트 변수 치환
    prompt = prompt_template.replace('{{ start_date }}', data['start_date'])
    prompt = prompt.replace('{{ end_date }}', data['end_date'])
    prompt = prompt.replace('{{ week_number }}', str(data['week_number']))
    prompt = prompt.replace('{{ year }}', str(data['year']))
    prompt = prompt.replace('{{ total_days }}', str(stats.get('total_days', 0)))
    prompt = prompt.replace('{{ total_commits }}', str(stats.get('total_commits', 0)))
    prompt = prompt.replace('{{ total_files }}', str(stats.get('total_files_changed', 0)))
    prompt = prompt.replace('{{ avg_commits }}', str(stats.get('avg_commits_per_day', 0)))
    prompt = prompt.replace('{{ avg_files }}', str(stats.get('avg_files_per_day', 0)))
    prompt = prompt.replace('{{ developer_count }}', str(stats.get('developer_count', 0)))
    prompt = prompt.replace('{{ developers }}', ', '.join(stats.get('developers', [])))
    prompt = prompt.replace('{{ daily_summaries }}', daily_summary_text)
    prompt = prompt.replace('{{ top_systems }}', systems_text)

    # OpenAI API 호출
    try:
        client = OpenAI(api_key=api_key)

        response = client.chat.completions.create(
            model=model,
            messages=[
                {
                    "role": "system",
                    "content": "당신은 소프트웨어 개발 팀의 주간 DevLog 작성을 돕는 AI 어시스턴트입니다."
                },
                {
                    "role": "user",
                    "content": prompt
                }
            ],
            temperature=0.7,
            max_tokens=3000
        )

        content = response.choices[0].message.content
        return content

    except Exception as e:
        print(f"[ERROR] OpenAI API 호출 실패: {e}")
        return None


def generate_devlog_fallback(data):
    """
    GPT 없이 기본 템플릿으로 주간 DevLog 생성

    Args:
        data: 주간 데이터

    Returns:
        str: 생성된 DevLog 본문
    """
    stats = data.get('stats', {})
    daily_summaries = data.get('daily_summaries', [])

    content = f"""## 1. 주간 요약

- 총 활동 일수: {stats.get('total_days', 0)}일
- 총 커밋 수: {stats.get('total_commits', 0)}개
- 총 변경 파일: {stats.get('total_files_changed', 0)}개
- 일평균 커밋: {stats.get('avg_commits_per_day', 0)}개
- 참여 개발자: {stats.get('developer_count', 0)}명

## 2. 일별 주요 변경

"""

    for summary in daily_summaries:
        content += f"### {summary['date']}\n\n{summary['summary']}\n\n"

    if not daily_summaries:
        content += "이번 주 Daily DevLog 없음\n\n"

    content += f"""## 3. 시스템별 변경 통계

"""

    top_systems = stats.get('top_systems', [])
    for system, count in top_systems[:10]:
        content += f"- **{system}**: {count}개 파일 변경\n"

    if not top_systems:
        content += "변경된 시스템 없음\n"

    content += f"""

## 4. 종합 분석

이번 주는 총 {stats.get('total_commits', 0)}개의 커밋으로 {stats.get('total_files_changed', 0)}개의 파일이 변경되었습니다.
참여 개발자는 {', '.join(stats.get('developers', []))}입니다.

## 5. 다음 주 계획

*(수동으로 작성 필요)*

## 6. 주요 이슈 및 개선 사항

*(수동으로 작성 필요)*
"""

    return content


def generate_weekly_devlog(data, config):
    """
    주간 DevLog 생성

    Args:
        data: 주간 데이터
        config: 설정 데이터

    Returns:
        str: 생성된 DevLog 본문
    """
    print("\n=== 주간 DevLog 생성 ===")

    # GPT API 사용 여부
    use_gpt = config.get('use_gpt', True)
    openai_api_key = config.get('openai_api_key') or os.getenv('OPENAI_API_KEY')

    if use_gpt and openai_api_key:
        print("GPT API를 사용하여 주간 DevLog 생성 중...")
        try:
            devlog_content = generate_weekly_devlog_with_gpt(
                data,
                api_key=openai_api_key,
                model=config.get('gpt_model', 'gpt-4o')
            )

            if devlog_content:
                print("  [OK] GPT 주간 DevLog 생성 완료")
                return devlog_content
            else:
                print("  [WARN] GPT 생성 실패, Fallback 사용")
                return generate_devlog_fallback(data)

        except Exception as e:
            print(f"  [WARN] GPT 오류: {e}, Fallback 사용")
            return generate_devlog_fallback(data)
    else:
        print("GPT API 미사용, Fallback 주간 DevLog 생성")
        return generate_devlog_fallback(data)


def save_weekly_devlog(devlog_content, data, config):
    """
    주간 DevLog를 파일로 저장

    Args:
        devlog_content: DevLog 본문
        data: 주간 데이터
        config: 설정 데이터

    Returns:
        Path: 저장된 파일 경로
    """
    project_root = Path(config.get('project_root', '.'))
    output_dir = project_root / config.get('weekly_output_dir', 'Documents/DevLog/Weekly')

    # 디렉토리 생성
    utils.ensure_dir(output_dir)

    # 파일명: YYYY-WXX-Summary.md (주 번호 기반)
    filename = f"{data['year']}-W{data['week_number']:02d}-Summary.md"
    output_path = output_dir / filename

    # 템플릿 파일 로딩
    template_path = script_dir / 'templates' / 'weekly_devlog_template.md'

    if template_path.exists():
        template_content = template_path.read_text(encoding='utf-8')

        stats = data.get('stats', {})

        # 템플릿 변수 치환
        devlog_full = template_content.replace('{{ year }}', str(data['year']))
        devlog_full = devlog_full.replace('{{ week_number }}', str(data['week_number']))
        devlog_full = devlog_full.replace('{{ start_date }}', data['start_date'])
        devlog_full = devlog_full.replace('{{ end_date }}', data['end_date'])
        devlog_full = devlog_full.replace('{{ total_days }}', str(stats.get('total_days', 0)))
        devlog_full = devlog_full.replace('{{ total_commits }}', str(stats.get('total_commits', 0)))
        devlog_full = devlog_full.replace('{{ total_files }}', str(stats.get('total_files_changed', 0)))
        devlog_full = devlog_full.replace('{{ developer_count }}', str(stats.get('developer_count', 0)))
        devlog_full = devlog_full.replace('{{ generated_time }}', datetime.now().strftime('%Y-%m-%d %H:%M:%S'))

        # GPT 콘텐츠를 템플릿의 첫 번째 구분선 이후에 삽입
        parts = devlog_full.split('---', 1)
        if len(parts) == 2:
            devlog_full = parts[0] + '---\n\n' + devlog_content + '\n\n---\n\n*생성 시간: ' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '*\n'
    else:
        # 템플릿이 없으면 기본 형식 사용
        stats = data.get('stats', {})
        devlog_full = f"""# Weekly DevLog — {data['year']} Week {data['week_number']}

**기간**: {data['start_date']} ~ {data['end_date']}
**활동 일수**: {stats.get('total_days', 0)}일
**총 커밋**: {stats.get('total_commits', 0)}개
**총 변경 파일**: {stats.get('total_files_changed', 0)}개
**참여 개발자**: {stats.get('developer_count', 0)}명

---

{devlog_content}

---

*생성 시간: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}*
"""

    # 파일 저장
    output_path.write_text(devlog_full, encoding='utf-8')

    # 메트릭 JSON 저장
    metrics_path = output_path.with_suffix('.metrics.json')
    metrics = {
        'year': data['year'],
        'week_number': data['week_number'],
        'start_date': data['start_date'],
        'end_date': data['end_date'],
        'branch': data.get('branch', 'main'),
        'stats': data.get('stats', {}),
        'generated_at': datetime.now().isoformat()
    }

    metrics_path.write_text(json.dumps(metrics, ensure_ascii=False, indent=2), encoding='utf-8')

    return output_path


def main():
    """메인 함수"""
    # config.env 파일이 있으면 로드
    project_env_file = script_dir.parent.parent / 'config.env'
    local_env_file = script_dir / 'config.env'

    if project_env_file.exists():
        load_env_file(project_env_file)
    elif local_env_file.exists():
        load_env_file(local_env_file)

    parser = argparse.ArgumentParser(
        description='KLingo 주간 DevLog 자동 생성 시스템'
    )

    parser.add_argument(
        '--config',
        default=None,
        help='설정 파일 경로 (기본: .github/config.json)'
    )

    parser.add_argument(
        '--date',
        default=None,
        help='기준 날짜 (YYYY-MM-DD 형식, 기본: 오늘 날짜). 해당 날짜가 속한 주의 DevLog 생성'
    )

    parser.add_argument(
        '--week',
        default=None,
        help='특정 주 번호 (예: 2025-W47). --date보다 우선'
    )

    parser.add_argument(
        '--branch',
        default='main',
        help='대상 브랜치명 (기본: main)'
    )

    args = parser.parse_args()

    # 프로젝트 루트 경로
    project_root = script_dir.parent.parent.parent

    # 설정 파일 경로
    if args.config:
        config_path = Path(args.config)
    else:
        config_path = project_root / '.github' / 'config.json'

    # 설정 로딩
    config = load_config(config_path)
    config.setdefault('project_root', str(project_root))

    # 기준 날짜 결정
    if args.week:
        # 주 번호로부터 날짜 계산 (예: 2025-W47)
        year, week = args.week.split('-W')
        target_date = date.fromisocalendar(int(year), int(week), 1)  # 해당 주의 월요일
    elif args.date:
        target_date = date.fromisoformat(args.date)
    else:
        target_date = date.today()

    # 주 범위 계산
    start_date, end_date = weekly_stats.get_week_range(target_date)

    print("=" * 60)
    print("  KLingo Weekly DevLog Auto-Generation System")
    print("=" * 60)

    # Daily DevLog 디렉토리 경로
    daily_dir = project_root / config.get('output_dir', 'Documents/DevLog/Daily')

    # 브랜치명
    branch_name = args.branch

    # 주간 데이터 수집
    data = weekly_stats.generate_weekly_summary_data(daily_dir, start_date, end_date, branch_name)

    # 주간 DevLog 생성
    devlog_content = generate_weekly_devlog(data, config)

    # 주간 DevLog 저장
    output_path = save_weekly_devlog(devlog_content, data, config)

    print("\n" + "=" * 60)
    print("  Weekly DevLog Generation Complete!")
    print("=" * 60)
    print(f"\n[*] Output: {output_path}")
    print(f"[*] Metrics: {output_path.with_suffix('.metrics.json')}")
    print(f"[*] Period: {data['start_date']} ~ {data['end_date']}")
    print(f"[*] Week: {data['year']}-W{data['week_number']:02d}")
    print(f"[*] Branch: {data['branch']}")


if __name__ == "__main__":
    main()
