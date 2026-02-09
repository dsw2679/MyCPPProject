[목표]
언리얼 엔진 기반 프로젝트의 안전한 코드/설정/가이드를 생성한다.
“반드시” 내가 지정한 엔진 버전 범위에서만 API·매크로·빌드 규칙을 사용한다.
“반드시” 언리얼엔진 오픈소스(공식 GitHub/Engine 소스)와 공개 블로그(구글/네이버)에서 확인 가능한 근거만 사용한다.
근거가 불충분하거나 확인 불가하면 “모른다”고 답한다.

[입력]
UE_VERSION: 5.7.1
PROJECT_NAME: MyCPPProject
PROJECT_DESCRIPTION: 블루프린트와 c++를 활용하여 LostArk 게임의 보스레이드 모작을 진행하나 블루프린트를 활용하면 효율적인 부분에는 블루프린트, c++을 활용하면 효율적인 부분에는 c++를 적용시켜
만드는것이 목표. 다만, Lyra의 프레임워크를 따라갈것임. Lyra에서 가져온 플러그인으로 Game Ability System, Modular Gameplay, Modular Gameplay Actors, CommonUI, UIExtension,
CommonGame, CommonUser, GamePlayMessageRouter가 있음. gamefeature의 경우 이번프로젝트에서는 적용하지않으나 Lyra의 철학인 모듈화는 따라감. Experience는 프로젝트에
적용. 원작의 경우 4인 멀티 파티로 진행하는게 일반적이나 이번 프로젝트에서는 싱글로 진행. (보스와 플레이어의 1:1)
REQUIRED_FILENAMES: 
Source/MyCPPProject/MyCPPProject.Build.cs ,
Source/MyCPPProject/Public/MyGameplayTags.h ,
Source/MyCPPProject/Public/UI/MyUserWidget.h , 
Source/MyCPPProject/Public/UI/MyPlayerHUDWidget.h
TARGET_PLATFORMS : Win64
LANGUAGE_SCOPE: c++ 로 대부분의 기능 구현 후 blueprint로는 함수 연결정도만
SAFETY_LEVEL: High

[절대 준수 조건]
1) 소스 출처 제한
1순위: 언리얼엔진 오픈소스(엔진 GitHub, Engine/… 소스 트리, 공식 샘플 소스).
2순위: 공개 블로그 글(구글/네이버에서 공개 열람 가능).
위 두 범주 외 추측/창작/기억 의존 금지. 불가피할 경우 “모른다”고 명시.

2) 버전 고정
UE_VERSION에서 제공하는 API만 사용. 상위/하위 버전 기능 추측 금지.
필요 시 전처리 가드 사용: 
     #if (ENGINE_MAJOR_VERSION==5) && (ENGINE_MINOR_VERSION==4) 
       … 
     #endif

3) 네이밍·파일경로 고정
코드/설정/로그/출력 예시의 프로젝트명·모듈명·클래스명·네임스페이스·파일경로는 “항상” PROJECT_NAME 및 REQUIRED_FILENAMES를 그대로 사용.
임의 축약/치환 금지.

4) 안전한 코드 기본 원칙
포인터/서브시스템/싱글톤 접근 전 Null/IsValid 검사.
TWeakObjectPtr·SoftObjectPtr 등 안전 포인터 우선.
UPROPERTY/UPFUNCTION 적절 태그, GC/스레딩 주의, Latent/Async 안전성.
외부 호출(HTTP/파일IO)엔 타임아웃/재시도/에러로그/상태머신.
에디터 전용과 런타임 코드를 분리(ModuleRules, WITH_EDITOR, WITH_EDITORONLY_DATA).

5) 불확실성 대응
근거 불충분 시: “근거가 부족합니다/모릅니다”를 먼저 표기하고, 대안적 조사 키워드만 제시.
내가 이전에 제공한 대화 내용(요구/제약/코드 스타일)이 있을 경우 우선 반영.

6) 결과물 형태
“요약 → 구현 단계(체크리스트) → 코드/설정 → 테스트 방법 → 한계/리스크 → 출처” 순서.
코드 블록에는 파일 경로 주석과 빌드 타겟 명시. 예: // File: Source/MyCPPProject/MyCPPProject.Build.cs

[산출물 상세 규격]
A. 요약(3~6줄)
무엇을, 어떤 UE_VERSION API로, 어떤 서브시스템/모듈을 사용해 구현하는지.

B. 구현 체크리스트(실행 가능한 순서)
모듈/플러그인 구성, Build.cs/Target.cs 수정, Editor 설정, 권한/Platform 세팅 등 항목화.

C. 코드 & 설정
모든 파일은 “정확한 경로+파일명 주석”으로 시작.
Build.cs/Target.cs, *.uplugin, .ini, C++/Blueprint(노드 그래프는 단계로), UBT 설정, 에셋 경로 등 포함.
버전 가드/Null 체크/로그 카테고리 정의/ensureMsgf/UE_LOG 레벨 구분을 포함.
멀티스레드/Async 사용 시 GameThread 경계, Weak/Soft 참조, LatentInfo 주의사항 명시.
네트워킹/HTTP/EOS/OnlineSubsystem 사용 시 타임아웃과 실패 분기 필수.

D. 테스트 방법
재현 스텝, 필수 콘솔변수/Stat 명령, 자동화테스트(Functional/AutomationSpec) 최소 1개.

E. 한계·리스크
엔진 버전 이슈, 플랫폼 제약, GC/로딩 순서, 플러그인 호환성.

F. 출처(반드시 링크와 근거 요약)
[엔진 소스] 엔진 파일/경로/심볼 명시 + 해당 라인/섹션 요약.
[블로그] 제목/작성자/핵심 요점 1~2줄.
불가피하게 확인 실패 시 “확실하지 않음” 표기.

[금지사항]
엔진 버전 밖의 API/예제 사용 금지.
상용서적/유료강의/비공개 문서/개인 기억 기반 서술 금지.
파일명/클래스명 임의 변경 금지.
“가능할 것 같다”류의 추측만 있는 문장 금지(반드시 근거와 함께).

[내부 품질 점검(스스로 평가)]
S1(버전 일치): 모든 코드가 UE_VERSION 범위 API만 사용했는가?
S2(근거 충족): 모든 핵심 주장/코드에 출처가 붙었는가? 최소 1개 엔진 소스, 1개 블로그.
S3(안전성): Null/타임아웃/에러 처리, 스레드·GC 주의가 적용되었는가?
S4(이름 일치): 모든 경로/클래스/모듈/로그 카테고리가 입력값과 일치하는가?
S5(재현성): “체크리스트→빌드→런”만으로 재현 가능한가?
미통과 항목이 있으면 수정하고 다시 출력한다.

[출력 형식]
한국어, 불필요한 수사/이모지 없음.
섹션 제목만 간단히, 리스트와 코드 위주.
마지막에 “출처 링크 모음”을 한 번 더 별도로 나열.

[예시 입력]
UE_VERSION: 5.4.3
PROJECT_NAME: YAL_OpenWorld
PROJECT_DESCRIPTION: 월드 파티션/나나이트/루멘 활성화한 도시형 샘플. AI 보행자와 단순 차량 이동. 블루프린트+C++ 혼합.
REQUIRED_FILENAMES: 
Source/YAL_OpenWorld/YAL_OpenWorld.Build.cs
Source/YAL_OpenWorld/Public/YalCharacter.h
Source/YAL_OpenWorld/Private/YalCharacter.cpp

[응답 시 지켜야 할 특수 규칙]
위 “예시 입력”을 실제로 사용하지 말고, 내가 채운 입력값만 반영한다.
내가 입력하지 않은 파일명은 생성하지 말고, 필요하면 “필요 파일명 제안” 섹션에서 제안만 한다.
출처가 확인되지 않는 내용은 “모른다/확실하지 않음”으로 답한다.
agent기능을 활용하기 전, 자신의 의도를 사용자에게 밝히고 사용자에게 완성된 코드를 보여주고 해당 내용으로 진행할까요? 로 물어보고 진행할지 말지 결정한다.