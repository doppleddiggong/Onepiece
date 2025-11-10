# Doxygen 문서화 및 GitHub Pages 배포 설정 가이드 (Project Onepiece)

## 1. 개요

이 가이드는 Unreal Engine C++ 프로젝트(`Project Onepiece`)에서 Doxygen을 사용하여 코드 문서를 생성하고, "Awesome Doxygen Theme"을 적용하며, GitHub Actions를 통해 자동으로 GitHub Pages에 배포하는 방법을 설명합니다. 실제 설정 과정에서 발생할 수 있는 문제점과 해결책을 포함하여, 처음부터 끝까지 성공적인 문서화 시스템을 구축하는 데 도움을 드립니다.

## 2. 전제 조건

*   **GitHub 저장소:** `Project Onepiece` 코드가 푸시되어 있는 GitHub 저장소.
*   **Doxygen:** 로컬에서 Doxygen을 테스트하려면 설치가 필요합니다. (예: `sudo apt-get install doxygen`)
*   **Graphviz:** 클래스 다이어그램 등 그래프 생성을 위해 필요합니다. (예: `sudo apt-get install graphviz`)

## 3. Doxyfile 생성 및 설정

`Doxyfile`은 Doxygen이 문서를 생성하는 방법을 정의하는 설정 파일입니다. 프로젝트 루트에 `Doxyfile`을 생성합니다.

```doxyfile
# Doxyfile for Onepiece Project Documentation

# Project related configuration
PROJECT_NAME           = "Onepiece Project Documentation"
PROJECT_BRIEF          = "Unreal Engine 5.6.1 C++ Project Documentation"
PROJECT_LOGO           =
OUTPUT_DIRECTORY       = docs
CREATE_SUBDIRS         = NO
OUTPUT_LANGUAGE        = English
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       = "The $name class"
ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_DOCS  = YES
FULL_PATH_NAMES        = YES
STRIP_FROM_PATH        =
STRIP_FROM_INC_PATH    =
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = NO
QT_AUTOBRIEF           = NO
MULTILINE_CPP_IS_BRIEF = NO
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
TAB_SIZE               = 4
OPTIMIZE_INPUT_CPP     = YES
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
EXTRACT_STATIC         = YES
EXTRACT_LOCAL_CLASSES  = YES
EXTRACT_LOCAL_METHODS  = YES
EXTRACT_ANON_NSPACES   = NO
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_INFO       = YES
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = NO
HIDE_SCOPE_NAMES       = NO
SHOW_INCLUDE_FILES     = YES
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_BRIEF_DOCS        = NO
SORT_GROUP_NAMES       = NO
SORT_BY_SCOPE_NAME     = NO
STRICT_PROTO_MATCH     = NO
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       =
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
SHOW_FILES_INDEX       = YES
SHOW_NAMESPACES        = YES
FILE_VERSION_FILTER    =
LAYOUT_FILE            =
CITE_BIB_FILES         =
Doxyfile_ENCODING      = UTF-8
LIST_OF_FILES          =
EXAMPLE_PATH           =
EXAMPLE_PATTERNS       = *
EXAMPLE_RECURSIVE      = YES
IMAGE_PATH             =
INPUT                  = Source
INPUT_ENCODING         = UTF-8
FILE_PATTERNS          = *.cpp *.h *.md
RECURSIVE              = YES
EXCLUDE                =
EXCLUDE_SYMBOLS        =
EXCLUDE_PATTERNS       =
EXCLUDE_USER_DEFINED_STYLESHEETS = NO
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          =
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            =
HTML_FOOTER            =
HTML_STYLESHEET        =
HTML_EXTRA_STYLESHEET  = doxygen-awesome.css
HTML_COLORSTYLE_HUE    = 220
HTML_COLORSTYLE_SAT    = 100
HTML_COLORSTYLE_GAMMA  = 80
HTML_TIMESTAMP         = YES
HTML_ALIGN_MEMBERS     = YES
HTML_DYNAMIC_SECTIONS  = NO
HTML_INDEX_NUM_FILES   = 100
GENERATE_HTMLHELP      = NO
CHM_FILE               =
HHC_LOCATION           =
GENERATE_CHI           = NO
CHM_INDEX_ENCODING     =
BINARY_TOC             = NO
TOC_EXPAND             = NO
GENERATE_RTF           = NO
GENERATE_MAN           = NO
GENERATE_XML           = NO
GENERATE_DOCBOOK       = NO
GENERATE_LATEX         = NO
DOT_IMAGE_FORMAT       = png
DOT_PATH               =
DOTFILE_DIRS           =
DOT_GRAPH_MAX_NODES    = 50
DOT_GRAPH_MAX_EDGES    = 20
DOT_CLEANUP            = YES
HAVE_DOT               = YES
CLASS_DIAGRAMS         = YES
UML_LOOK               = NO
UML_LIMIT_NUM_FIELDS   = 10
DOT_FONTNAME           = Helvetica
DOT_FONTSIZE           = 10
DOT_FONTPATH           =
DOT_MULTI_TARGETS      = NO
GENERATE_LEGEND        = YES
GRAPH_COLOR            = NO
INCLUDE_GRAPH          = YES
ENABLED_SECTIONS       =
REFERENCED_BY_RELATION = YES
REFERENCES_RELATION    = YES
CALL_GRAPH             = NO
CALLER_GRAPH           = NO
GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = YES
```

### AgentRule 문서 비포함

에이전트별 가이드는 내부 업무 참고용이며, 공개 API 문서에는 노출하지 않습니다. 따라서 `INPUT` 항목에는 `AgentRule/` 디렉터리와 루트의
에이전트 가이드(`AGENTS.md`, `CLAUDE.md`, `GEMINI.md`)를 추가하지 마세요. 기본 Doxyfile은 게임 소스와 공개 참고 자료(`Documents`
폴더 내 DoxygenPage/Reference)만 포함하도록 구성되어 있습니다.

필요에 따라 로컬에서 검토하고 싶다면 임시 브랜치에서만 `INPUT` 목록을 수정하고, 메인 브랜치에 병합하기 전 반드시 되돌려 공용 문서에
에이전트 자료가 노출되지 않도록 합니다.

## 4. GitHub Actions 워크플로우 (`doxygen.yml`) 설정

`.github/workflows/doxygen.yml` 파일을 생성하여 Doxygen 문서 생성 및 GitHub Pages 배포를 자동화합니다.

```yaml
name: Generate Doxygen

on:
  push:
    branches:
      - main
    paths:
      - 'Source/**'
      - 'Plugins/**'
      - '**/*.md'
      - 'Doxyfile'
  pull_request:
    paths:
      - 'Source/**'
      - 'Plugins/**'
      - '**/*.md'
      - 'Doxyfile'

permissions:
  contents: write

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - name: Checkout repository
      uses: actions/checkout@v4

    - name: Install Doxygen
      run: sudo apt-get update && sudo apt-get install -y doxygen graphviz

    - name: Download Doxygen Awesome CSS
      run: |-
        wget https://raw.githubusercontent.com/jothepro/doxygen-awesome-css/main/doxygen-awesome.css
        mkdir -p docs/html
        mv doxygen-awesome.css docs/html/

    - name: Generate Doxygen documentation
      run: doxygen Doxyfile

    - name: Upload documentation artifact
      uses: actions/upload-artifact@v4
      with:
        name: Doxygen-Documentation
        path: docs/html

    - name: Deploy to GitHub Pages
      uses: peaceiris/actions-gh-pages@v3
      if: ${{ github.ref == 'refs/heads/main' }}
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: ./docs/html
        publish_branch: gh-pages
        destination_dir: doxygen
        enable_jekyll: false
        force_orphan: false
        keep_files: true
```

## 5. GitHub 저장소 설정

### 5.1. GitHub Actions 권한 설정

`github-actions[bot]`이 `gh-pages` 브랜치에 푸시할 수 있도록 권한을 부여해야 합니다.

1.  GitHub 저장소 웹페이지로 이동합니다.
2.  저장소 상단의 **"Settings" (설정)** 탭을 클릭합니다.
3.  왼쪽 사이드바에서 **"Actions"** 아래의 **"General"**을 클릭합니다.
4.  페이지를 아래로 스크롤하여 **"Workflow permissions" (워크플로우 권한)** 섹션을 찾습니다.
5.  **"Read and write permissions" (읽기 및 쓰기 권한)** 옵션을 선택합니다.
6.  **"Save" (저장)** 버튼을 클릭합니다.

### 5.2. GitHub Pages 소스 설정

GitHub Pages가 `gh-pages` 브랜치에서 문서를 제공하도록 설정합니다.

1.  GitHub 저장소 웹페이지로 이동합니다.
2.  저장소 상단의 **"Settings" (설정)** 탭을 클릭합니다.
3.  왼쪽 사이드바에서 **"Pages"**를 클릭합니다.
4.  "Build and deployment" 섹션에서 **"Source"** 드롭다운 메뉴를 클릭하여 **"Deploy from a branch"**를 선택합니다.
5.  **"Branch"** 드롭다운 메뉴에서 **`gh-pages`**를 선택합니다.
6.  폴더는 **`/ (root)`**로 그대로 둡니다.
7.  **"Save" (저장)** 버튼을 클릭합니다.

## 6. 배포 확인 및 문제 해결 (Troubleshooting)

설정 과정에서 발생할 수 있는 일반적인 문제와 해결책입니다.

### 6.1. 문제 1: `wget` 404 Not Found 오류

*   **원인:** `awesome-doxygen-css` 테마 파일의 GitHub Raw URL이 변경되었을 수 있습니다.
*   **해결:** `.github/workflows/doxygen.yml` 파일 내의 `wget` URL을 최신 URL로 업데이트합니다. (현재는 `https://raw.githubusercontent.com/jothepro/doxygen-awesome-css/main/doxygen-awesome.css`가 올바릅니다.)

### 6.2. 문제 2: "Deploy to GitHub Pages" 단계가 실행되지 않음

*   **원인:** `doxygen.yml` 워크플로우의 `Deploy to GitHub Pages` 단계에 `if: ${{ github.ref == 'refs/heads/main' }}` 조건이 있어, `main` 브랜치가 아닌 다른 브랜치에서 푸시가 발생하면 해당 단계가 건너뛰어집니다.
*   **해결:** 변경사항을 `main` 브랜치로 푸시하거나, `main` 브랜치로 병합합니다.

### 6.3. 문제 3: `Permission denied` 오류 (403 Forbidden)

*   **원인:** `github-actions[bot]`에 저장소에 변경사항을 푸시할 권한이 부족합니다.
*   **해결:** GitHub 저장소 설정 → Actions → General → Workflow permissions에서 "Read and write permissions" 옵션을 선택하고 저장합니다. 또는 워크플로우 YAML 파일에 `permissions: contents: write`를 추가합니다.

### 6.4. 문제 4: GitHub Pages 404 오류 (사이트 주소는 맞는데 내용이 안 보임)

*   **원인 1: GitHub Pages "Source" 설정이 "GitHub Actions"로 되어 있음.**
    *   **해결 1:** GitHub 저장소 설정 → Pages → "Build and deployment" → "Source"를 "Deploy from a branch"로 변경 후 `gh-pages` 브랜치와 `/ (root)` 폴더를 선택합니다.
*   **원인 2: GitHub Pages "Source" 설정에서 폴더가 `/docs`로 잘못 지정됨.**
    *   **해결 2:** GitHub 저장소 설정 → Pages → "Branch" 옆의 드롭다운 메뉴에서 `/ (root)`로 변경합니다.
*   **원인 3: 캐싱 문제 또는 전파 지연.**
    *   **해결 3:** 브라우저 캐시를 지우거나, 시크릿 창을 사용하거나, 다른 브라우저로 접속해 봅니다. GitHub Pages는 변경사항이 전파되는 데 시간이 걸릴 수 있으므로 잠시 기다려 봅니다.

## 7. 결론

모든 설정이 완료되고 워크플로우가 성공적으로 실행되면, 다음 주소에서 Doxygen 문서를 확인할 수 있습니다: `https://[사용자이름].github.io/Onepiece/doxygen/`

## 8. Doxygen 주석 가이드라인 (Doxygen Commenting Guidelines)

Doxygen을 통해 일관되고 유용한 문서를 생성하기 위해 다음 주석 가이드라인을 준수합니다.

### 8.1. 일반 원칙

*   모든 공개(Public) 클래스, 구조체, 열거형, 함수, 멤버 변수에는 Doxygen 주석을 추가합니다.
*   주석은 코드의 목적, 사용법, 중요한 세부 사항을 명확하게 설명해야 합니다.
*   간결하면서도 정보가 풍부하도록 작성합니다.

### 8.2. 주석 스타일 및 위치

*   **블록 주석 (Block Comments):** `/** ... */` 스타일을 사용하며, 여러 줄에 걸친 설명이나 Doxygen 명령어를 포함할 때 사용합니다.
    *   클래스, 구조체, 열거형, 함수 정의 바로 위에 위치합니다.
*   **한 줄 주석 (Brief Comments):** `///` 스타일을 사용하며, 짧은 설명이나 멤버 변수 옆에 인라인으로 사용할 때 유용합니다.

### 8.3. 주요 Doxygen 명령어

*   `@brief <설명>`: 요소에 대한 간결한 한 줄 요약입니다. 모든 주요 요소(클래스, 함수 등)에 포함되어야 합니다.
*   `@details <설명>`: 요소에 대한 상세한 설명입니다. `@brief`로 충분하지 않을 때 사용합니다.
*   `@param <매개변수명> <설명>`: 함수의 각 매개변수에 대한 설명입니다.
*   `@return <설명>`: 함수의 반환 값에 대한 설명입니다.
*   `@see <다른 요소>`: 관련 있는 다른 클래스, 함수, 파일 등을 참조할 때 사용합니다.
*   `@note <내용>`: 특별히 주의해야 할 사항을 명시합니다.
*   `@warning <내용>`: 잠재적인 문제점이나 위험을 경고합니다.
*   `@todo <내용>`: 향후 구현 또는 개선이 필요한 사항을 기록합니다.
*   `@file <파일명>`: 헤더 파일의 최상단에 파일의 목적을 설명할 때 사용합니다.

### 8.4. 예시

#### 클래스 주석 예시

```cpp
/**
 * @brief 플레이어 캐릭터의 기본 액터 클래스입니다.
 *
 * @details 이 클래스는 플레이어의 움직임, 상호작용, 전투 등 핵심 게임플레이 로직을 담당합니다.
 *          주요 기능으로는 입력 처리, 스탯 관리 시스템과의 연동 등이 있습니다.
 *
 * @see UStatSystem
 * @see UKnockbackSystem
 */
UCLASS()
class LATTELIBRARY_API APlayerActor : public ACharacter
{
    // ...
};
```

#### 함수 주석 예시

```cpp
/**
 * @brief 캐릭터의 현재 체력을 반환합니다.
 *
 * @param bIncludeTemporaryHealth 임시 체력을 포함할지 여부입니다.
 * @return 현재 체력 값을 반환합니다.
 */
float GetCurrentHealth(bool bIncludeTemporaryHealth) const;
```

#### 멤버 변수 주석 예시

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
float MaxHealth; ///< 캐릭터의 최대 체력입니다.
```

### 8.5. 파일 분할 고려사항

*   **단일 책임 원칙 (Single Responsibility Principle):** 하나의 헤더 파일은 하나의 주요 클래스 또는 밀접하게 관련된 소수의 엔티티만을 포함하도록 노력합니다.
*   **모듈성:** 큰 클래스나 복잡한 시스템은 여러 개의 작은 클래스/컴포넌트로 분할하고, 각각의 헤더 파일에 적절한 주석을 추가하여 Doxygen이 개별적으로 문서를 생성할 수 있도록 합니다. 이는 Doxygen 문서의 가독성을 높일 뿐만 아니라 코드의 유지보수성도 향상시킵니다.

---

## 9. GitHub Actions + GitHub Pages 배포 문제 해결 기록

### 문제 상황

GitHub Actions를 사용하여 Doxygen 문서를 자동 생성 후 `gh-pages` 브랜치로 배포하려 했으나,
다음과 같은 에러가 발생함.

```
remote: Permission to [사용자명]/Onepiece.git denied to github-actions[bot].
fatal: unable to access 'https://github.com/[사용자명]/Onepiece.git/': The requested URL returned error: 403
Error: Action failed with "The process '/usr/bin/git' failed with exit code 128"
```

즉, GitHub Actions 봇이 `gh-pages` 브랜치에 push 권한을 가지지 못해 배포에 실패한 상태.

### 원인 분석

1. 기본 제공되는 `GITHUB_TOKEN`은 **읽기 전용** 권한만 가지고 있음.
2. `gh-pages` 브랜치에 Branch Protection Rule이 설정되어 있으면 push 불가.
3. Pages 설정에서 Source는 `gh-pages`로 잘 잡혀 있었으나, Actions의 권한 부족으로 인해 배포가 막힘.

### 해결 과정

1. **워크플로우 권한 부여**
   - workflow yaml 상단에 다음 코드 추가:
   ```yaml
   permissions:
     contents: write
   ```

2. **배포 스텝 확인**
   - `peaceiris/actions-gh-pages@v3` 스텝에서 `github_token`을 그대로 사용:
   ```yaml
   - name: Deploy to GitHub Pages
     uses: peaceiris/actions-gh-pages@v3
     if: ${{ github.ref == 'refs/heads/main' }}
     with:
       github_token: ${{ secrets.GITHUB_TOKEN }}
       publish_dir: ./docs/html
       destination_dir: doxygen
   ```

3. **브랜치 보호 규칙 점검**
   - `gh-pages` 브랜치에 보호 규칙이 걸려 있으면 push 불가 → 현재는 보호 규칙 없음.

4. **GitHub Pages 설정 확인**
   - Repository Settings → Pages → Source가 `gh-pages`로 정상 지정됨.
   - HTTPS 강제 적용됨.

### 최종 결과

- `permissions: contents: write` 추가 후 Action이 정상적으로 `gh-pages` 브랜치에 push 가능.
- 빌드 완료 후 Doxygen 문서 확인 가능.

### 교훈

- GitHub Actions에서 push 작업을 하려면 `permissions: contents: write`를 꼭 추가해야 한다.
- Branch Protection Rule이 있으면 Action이 실패하므로 필요시 해제하거나 별도 규칙으로 조정한다.
- GitHub Pages Source는 반드시 `gh-pages`로 지정되어야 하며, Custom domain 사용 시 HTTPS 설정을 별도로 확인해야 한다.
