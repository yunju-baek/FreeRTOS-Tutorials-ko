# FreeRTOS-Tutorials-ko 작업 규칙

- 정본은 `origin/main`, 공식 영문 원격은 `upstream`이다. `PROJECT_STATE.md`, `ko/TRANSLATION_GUIDE.md`, `ko/WORKFLOW.md`, `ko/REVIEW_NOTES.md`를 읽고 요청 범위를 확인한다.
- 한국어 문서는 `ko/`, 영문 원문은 `docs/`와 `source/`다. 루트 README만 한국어판 안내로 바꾸며 원문 README는 `README.upstream.md`에 byte 단위로 보존한다.
- upstream 기준은 manifest와 `upstream-baseline` branch다. 영문 원본·C source·설정·TODO·라이선스·submodule을 보존한다. 원격 fetch만으로 번역 원문 기준을 바꾸지 않는다.
- 한다체와 커널북의 지정 용어집을 따른다. 영어 객체명은 첫 한국어 풀이를 붙이고 같은 영어를 반복하지 않는다. 캡션 번호는 한국어만 사용한다. 독자용 페이지에는 작업용 front matter를 넣지 않는다.
- 코드·주석·문자열·로그를 번역하지 않는다. 원문 밖의 설명과 수정 근거는 구분해 기록한다.
- 검수는 원문·코드·구조·표시·실행을 구분하고 실제 범위만 기록한다. 코드 실행이나 사람 검토를 수행했다고 대신 표시하지 않는다.
- 작업 branch에서 검수·commit·push 후 한국어판 저장소에 PR을 제출한다. 사용자 요청 또는 명시적 규칙에 따라 병합한다.
- Codex·Claude 세션·설정·인증 파일, 비밀키, 머신별 환경과 캐시는 저장소에 넣지 않는다. Python 환경과 node_modules는 iCloud 밖에 둔다. 원본·검수 전 작업본·기존 다른 저장소는 보존한다.
