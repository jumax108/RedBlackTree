// treeViewer.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "treeViewer.h"

#include "SimpleProfiler.h"
#include "RedBlackTree.h"

#if defined(SPEED_TEST)
    SimpleProfiler* sp = nullptr;
#endif

using TREE_CLASS = CRedBlackTree<int>;

TREE_CLASS* tree;

using namespace std;
vector<int> addValue;

RECT windowRect;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TREEVIEWER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TREEVIEWER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TREEVIEWER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(CreateSolidBrush(RGB(20,120,50)));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TREEVIEWER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   AllocConsole();
   freopen_s((FILE**)stdout,"CONOUT$", "w", stdout);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
int value;
int index;

int x = 0;

int inputCnt;
WCHAR input[3] = {0, };

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            #if defined(SPEED_TEST)
                sp = new SimpleProfiler();
            #endif

            tree = new TREE_CLASS();
            srand(500);

            GetWindowRect(hWnd, &windowRect);
            windowRect.right -= windowRect.left;
            windowRect.bottom -= windowRect.top;
            windowRect.left = 0;
            windowRect.top = 0;
            InvalidateRect(hWnd, nullptr, true);

        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            tree->print(hdc, x);
            TextOutW(hdc, 0, 0, input, inputCnt);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_KEYDOWN: {
        if (wParam == VK_BACK) {
            if (inputCnt == 0) {
                break;
            }
            inputCnt -= 1;
            input[inputCnt] = NULL;
            break;
        }
        if (inputCnt == 2) {
            break;
        }
        if ('0' > wParam || wParam > '9') {
            break;
        }
        input[inputCnt++] = wParam;
        InvalidateRect(hWnd, nullptr, true);
    }
        break;
    case WM_CHAR:
        switch (wParam) {

            case 'a':
            {
                x -= 10;
            }
            break;
            
            case 'd':
            {
                x += 10;
            }
            break;

            case 'z':
            {
                // random value add
                value = rand() % 99 + 1;

                if (value == 5) {
                    int k = 3;
                }

                tree->insert(value);
                addValue.push_back(value);
                
            }
            break;

            case 'x':
            {
                // random value erase
                if (addValue.size() == 0) {
                    break;
                }
                
                index = rand() % addValue.size();
                if (addValue[index] == 88) {
                    int k = 3;
                }
                tree->erase(addValue[index]);
                addValue.erase(addValue.begin() + index);
                
            }
            break;

            case 'c':
            {
                
                srand(1008);
                for (int loopCnt = 0; loopCnt < 1000 ; ++loopCnt ) {
                    delete(tree);
                    tree = TREE_CLASS::test();

                    for (int i = 0; i < 100; i++) {
                        int result = tree->find(i);
                        wprintf(L"find %d, result: %d\n", i, result);
                    }

                    int* treeArr = tree->inorder();
                    int nodeNum = tree->getNodeNum();
                    for (int nodeCnt = 0; nodeCnt < nodeNum; nodeCnt++) {
                        wprintf(L"%d ", treeArr[nodeCnt]);
                    }

                }


                #if defined(SPEED_TEST)
                    sp->printToFile();
                #endif
            }

            break;

            case 'q': {
                // 선택한 값 삽입
                int value = _wtoi(input);

                tree->insert(value);
                addValue.push_back(value);

                inputCnt = 0;

            }
                break;

            case 'e': {
                // 선택한 값 제거

                int value = _wtoi(input);

                tree->erase(value);
                std::vector<int>::iterator iter;
                for (iter = addValue.begin(); iter != addValue.end(); ++iter) {
                    if (*iter == value) {
                        break;
                    }
                }
                if (iter != addValue.end()) {
                    addValue.erase(addValue.begin() + index);
                }
                inputCnt = 0;
            }
                break;
        }
        InvalidateRect(hWnd, nullptr, true);
        break;
    case WM_DESTROY:
        delete(tree);
        #if defined(SPEED_TEST)
            delete(sp);
        #endif
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
