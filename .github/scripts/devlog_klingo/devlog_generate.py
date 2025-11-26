#!/usr/bin/env python3
"""
KLingo DevLog 자동 생성 시스템
Git diff, 커밋 메시지, 회의록, Jira를 통합하여 Daily DevLog 생성
"""

import argparse
import json
import os
import sys
from pathlib import Path
from datetime import datetime

# 현재 스크립트 디렉토리를 모듈 경로에 추가
script_dir = Path(__file__).parent
sys.path.insert(0, str(script_dir))

# 로컬 모듈 임포트
import utils
import meeting_loader
import diff_loader
import jira_mapper
import gpt_client


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
        print(f"[WARN]  설정 파일을 찾을 수 없습니다: {config_path}")
        return {}

    with open(config_file, 'r', encoding='utf-8') as f:
        return json.load(f)


def collect_data(config, target_date, branch_name):
    """
    DevLog 생성에 필요한 모든 데이터 수집

    Args:
        config: 설정 데이터
        target_date: 대상 날짜 (date 객체)
        branch_name: Git 브랜치명

    Returns:
        dict: 수집된 데이터
    """
    project_root = Path(config.get('project_root', '.'))

    print(f"\n=== 데이터 수집 시작 ===")
    print(f"날짜: {target_date}")
    print(f"브랜치: {branch_name}")

    data = {
        'date': target_date.isoformat(),
        'branch': branch_name
    }

    # 1. 개발자 정보 로딩
    print("\n[1/6] 개발자 정보 로딩...")
    developers_path = project_root / config.get('developers_csv', '.github/data/developers.csv')
    developers = utils.load_developers(developers_path)

    developer_info = utils.get_developer_from_branch(branch_name, developers)
    if developer_info:
        data['developer'] = developer_info['developer']
        data['jira_account'] = developer_info['jira_account']
        print(f"  [OK] 개발자: {developer_info['developer']}")
    elif branch_name == 'main':
        data['developer'] = '전체 팀'
        data['jira_account'] = None
        print(f"  [OK] 개발자: 전체 팀 (main 브랜치)")
    else:
        data['developer'] = 'Unknown'
        data['jira_account'] = None
        print(f"  [WARN]  브랜치 '{branch_name}'에 대한 개발자 정보를 찾을 수 없습니다")

    # 2. 회의록 로딩
    print("\n[2/6] 회의록 로딩...")
    meeting_dir = project_root / config.get('meeting_dir', 'Documents/Meeting/Daily')
    meeting = meeting_loader.find_meeting_by_date(meeting_dir, target_date)

    if meeting:
        data['meeting'] = meeting
        print(f"  [OK] 회의록: {meeting['title']}")
    else:
        data['meeting'] = None
        print(f"  [WARN]  회의록을 찾을 수 없습니다")

    # 3. Git diff 및 커밋 로딩
    print("\n[3/6] Git diff 및 커밋 로딩...")
    since, until = utils.get_git_date_range(target_date)

    commits = diff_loader.get_commits_in_range(since, until, project_root)
    commit_messages = diff_loader.get_commit_messages_in_range(since, until, project_root)
    changed_files = diff_loader.get_changed_files_in_range(since, until, project_root)
    diff = diff_loader.get_diff_in_range(since, until, project_root)

    data['commits'] = commit_messages
    data['changed_files'] = changed_files
    data['diff'] = diff

    print(f"  [OK] 커밋: {len(commits)}개")
    print(f"  [OK] 변경 파일: {len(changed_files)}개")
    print(f"  [OK] Diff 크기: {len(diff)} 문자")

    # 4. Jira 매핑
    print("\n[4/6] Jira 시스템 매핑...")
    jira_rules_path = project_root / config.get('jira_rules', '.github/data/jira_map_rules.yaml')
    jira_rules = jira_mapper.load_jira_rules(jira_rules_path)

    system_counter = jira_mapper.map_files_to_systems(changed_files, jira_rules)
    top_systems = jira_mapper.get_top_systems(system_counter, max_count=5)

    data['top_systems'] = top_systems
    data['mapping_rules'] = jira_rules

    print(f"  [OK] 변경된 시스템: {len(system_counter)}개")
    if top_systems:
        for system, count in top_systems:
            print(f"    - {system}: {count}개 파일")

    # 5. Jira API 호출
    print("\n[5/6] Jira 이슈 조회...")
    jira_config = config.get('jira', {})

    # JIRA_SITE가 있으면 URL 자동 구성
    jira_site = os.getenv('JIRA_SITE')
    if jira_site:
        jira_url = f"https://{jira_site}.atlassian.net"
    else:
        jira_url = jira_config.get('url') or os.getenv('JIRA_URL')

    jira_email = jira_config.get('email') or os.getenv('JIRA_EMAIL')
    jira_token = jira_config.get('token') or os.getenv('JIRA_TOKEN')
    jira_project = jira_config.get('project') or os.getenv('JIRA_PROJECT')

    if config.get('jira', {}).get('use_jira', True) and jira_url and jira_email and jira_token:
        try:
            # main 브랜치인 경우: 모든 개발자의 Jira 이슈 조회
            if branch_name == 'main':
                print(f"  [*] main 브랜치: 전체 개발자 Jira 이슈 조회")
                all_jira_issues = []

                # developers.csv에서 모든 개발자의 jira_account 가져오기
                for dev in developers.values():
                    dev_jira_account = dev.get('jira_account')
                    if dev_jira_account:
                        try:
                            issues = jira_mapper.fetch_jira_issues(
                                jira_url, jira_email, jira_token,
                                dev_jira_account,
                                jira_project=jira_project,
                                max_results=10
                            )
                            # 개발자 정보 추가
                            for issue in issues:
                                issue['developer'] = dev.get('developer', 'Unknown')
                            all_jira_issues.extend(issues)
                            print(f"      - {dev.get('developer')}: {len(issues)}개 이슈")
                        except Exception as e:
                            print(f"      - {dev.get('developer')}: 조회 실패 ({e})")

                data['jira_issues'] = all_jira_issues
                print(f"  [OK] 전체 Jira 이슈: {len(all_jira_issues)}개")
                if jira_project:
                    print(f"       프로젝트: {jira_project}")

            # 개인 브랜치인 경우: 해당 개발자의 Jira 이슈만 조회
            elif data.get('jira_account'):
                jira_issues = jira_mapper.fetch_jira_issues(
                    jira_url, jira_email, jira_token,
                    data['jira_account'],
                    jira_project=jira_project,
                    max_results=10
                )
                data['jira_issues'] = jira_issues
                print(f"  [OK] Jira 이슈: {len(jira_issues)}개")
                if jira_project:
                    print(f"       프로젝트: {jira_project}")
            else:
                print(f"  [WARN]  개발자 정보를 찾을 수 없습니다")
                data['jira_issues'] = []
        except Exception as e:
            print(f"  [WARN]  Jira API 호출 실패: {e}")
            data['jira_issues'] = []
    else:
        print(f"  [WARN]  Jira 설정이 없습니다 (환경 변수 또는 config.json 설정)")
        data['jira_issues'] = []

    # 6. 데이터 수집 완료
    print("\n[6/6] 데이터 수집 완료")

    return data


def generate_devlog(data, config):
    """
    수집된 데이터로 DevLog 생성

    Args:
        data: collect_data() 결과
        config: 설정 데이터

    Returns:
        str: 생성된 DevLog 본문
    """
    print("\n=== DevLog 생성 ===")

    # GPT API 사용 여부
    use_gpt = config.get('use_gpt', True)
    openai_api_key = config.get('openai_api_key') or os.getenv('OPENAI_API_KEY')

    if use_gpt and openai_api_key:
        print("GPT API를 사용하여 DevLog 생성 중...")
        try:
            devlog_content = gpt_client.generate_devlog_with_gpt(
                data,
                api_key=openai_api_key,
                model=config.get('gpt_model', 'gpt-4o')
            )

            if devlog_content:
                print("  [OK] GPT DevLog 생성 완료")
                return devlog_content
            else:
                print("  [WARN]  GPT 생성 실패, Fallback 사용")
                return gpt_client.generate_devlog_fallback(data)

        except Exception as e:
            print(f"  [WARN]  GPT 오류: {e}, Fallback 사용")
            return gpt_client.generate_devlog_fallback(data)
    else:
        print("GPT API 미사용, Fallback DevLog 생성")
        return gpt_client.generate_devlog_fallback(data)


def save_devlog(devlog_content, data, config):
    """
    DevLog를 파일로 저장

    Args:
        devlog_content: DevLog 본문
        data: 수집된 데이터
        config: 설정 데이터

    Returns:
        Path: 저장된 파일 경로
    """
    project_root = Path(config.get('project_root', '.'))
    output_dir = project_root / config.get('output_dir', 'Documents/DevLog/Daily')

    # 디렉토리 생성
    utils.ensure_dir(output_dir)

    # 파일명: YYYY-MM-DD.md
    filename = f"{data['date']}.md"
    output_path = output_dir / filename

    # 템플릿 파일 로딩
    template_path = script_dir / 'templates' / 'devlog_template.md'
    if template_path.exists():
        template_content = template_path.read_text(encoding='utf-8')

        # 템플릿 변수 치환
        devlog_full = template_content.replace('{{ date }}', data['date'])
        devlog_full = devlog_full.replace('{{ developer }}', data.get('developer', 'Unknown'))
        devlog_full = devlog_full.replace('{{ branch }}', data.get('branch', 'Unknown'))
        devlog_full = devlog_full.replace('{{ commit_count }}', str(len(data.get('commits', []))))
        devlog_full = devlog_full.replace('{{ files_changed }}', str(len(data.get('changed_files', []))))
        devlog_full = devlog_full.replace('{{ generated_time }}', datetime.now().strftime('%Y-%m-%d %H:%M:%S'))

        # GPT 생성 내용 삽입 (섹션별로 분리되어 있다고 가정)
        devlog_full = devlog_full.replace('{{ top_changes }}', '')
        devlog_full = devlog_full.replace('{{ impact }}', '')
        devlog_full = devlog_full.replace('{{ risks }}', '')
        devlog_full = devlog_full.replace('{{ verification }}', '')
        devlog_full = devlog_full.replace('{{ meeting_todo }}', '')
        devlog_full = devlog_full.replace('{{ jira_candidates }}', '')

        # GPT 콘텐츠를 템플릿의 첫 번째 구분선 이후에 삽입
        parts = devlog_full.split('---', 1)
        if len(parts) == 2:
            devlog_full = parts[0] + '---\n\n' + devlog_content + '\n\n---\n\n*생성 시간: ' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '*\n'
    else:
        # 템플릿이 없으면 기존 방식 사용
        devlog_full = f"""# Daily DevLog — {data['date']}

**개발자**: {data.get('developer', 'Unknown')}
**브랜치**: {data.get('branch', 'Unknown')}
**커밋 수**: {len(data.get('commits', []))}개
**변경 파일**: {len(data.get('changed_files', []))}개

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
        'date': data['date'],
        'developer': data.get('developer'),
        'branch': data.get('branch'),
        'commit_count': len(data.get('commits', [])),
        'files_changed': len(data.get('changed_files', [])),
        'top_systems': data.get('top_systems', []),
        'generated_at': datetime.now().isoformat()
    }

    metrics_path.write_text(json.dumps(metrics, ensure_ascii=False, indent=2), encoding='utf-8')

    return output_path


def main():
    """메인 함수"""
    # config.env 파일이 있으면 로드 (우선순위: 프로젝트 루트 > 스크립트 디렉토리)
    project_env_file = script_dir.parent.parent / 'config.env'
    local_env_file = script_dir / 'config.env'

    if project_env_file.exists():
        load_env_file(project_env_file)
    elif local_env_file.exists():
        load_env_file(local_env_file)

    parser = argparse.ArgumentParser(
        description='KLingo DevLog 자동 생성 시스템'
    )

    parser.add_argument(
        '--config',
        default=None,
        help='설정 파일 경로 (기본: .github/config.json)'
    )

    parser.add_argument(
        '--date',
        default=None,
        help='대상 날짜 (YYYY-MM-DD 형식, 기본: 어제)'
    )

    parser.add_argument(
        '--branch',
        default=None,
        help='Git 브랜치명 (기본: main)'
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

    # 대상 날짜
    if args.date:
        from datetime import date
        target_date = date.fromisoformat(args.date)
    else:
        target_date = utils.get_devlog_date()

    # 브랜치명 (기본값: main)
    if args.branch:
        branch_name = args.branch
    else:
        branch_name = 'main'

    print("=" * 50)
    print("  KLingo DevLog Auto-Generation System")
    print("=" * 50)

    # 데이터 수집
    data = collect_data(config, target_date, branch_name)

    # DevLog 생성
    devlog_content = generate_devlog(data, config)

    # DevLog 저장
    output_path = save_devlog(devlog_content, data, config)

    print("\n" + "=" * 50)
    print("  DevLog Generation Complete!")
    print("=" * 50)
    print(f"\n[*] Output: {output_path}")
    print(f"[*] Metrics: {output_path.with_suffix('.metrics.json')}")


if __name__ == "__main__":
    main()
