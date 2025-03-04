
#include <Windows.h>
#include <tchar.h>
#include <thread>
#include <chrono>
#include <GLFW/glfw3.h>

// ȫ�ֱ���
HINSTANCE g_hInst = nullptr;
HWND g_hSplashWnd = nullptr;
volatile float g_progress = 0.0f;  // 0.0~1.0

// Splash Screen �Ĵ��ڹ��̣����ڻ��ƺʹ�����Ϣ
LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);

        // ��䱳��
        HBRUSH hBrushBack = (HBRUSH)GetStockObject(WHITE_BRUSH);
        FillRect(hdc, &rect, hBrushBack);

        // ���Ƽ����ı�
        const TCHAR* loadingText = _T("Loading, please wait...");
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, loadingText, -1, &rect, DT_CENTER | DT_TOP);

        // ���ƽ�����������һ��������ģ��
        int barMargin = 50;
        int barHeight = 20;
        int barWidth = rect.right - 2 * barMargin;
        int filledWidth = static_cast<int>(barWidth * g_progress);
        RECT progressRect = { rect.left + barMargin, rect.bottom - barMargin - barHeight, rect.left + barMargin + filledWidth, rect.bottom - barMargin };

        HBRUSH hBrushProgress = CreateSolidBrush(RGB(0, 120, 215));
        FillRect(hdc, &progressRect, hBrushProgress);
        DeleteObject(hBrushProgress);

        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// Splash Screen �߳���ں���
DWORD WINAPI SplashScreenThread(LPVOID lpParam)
{
    g_hInst = GetModuleHandle(NULL);

    // ע�ᴰ����
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc   = SplashWndProc;
    wc.hInstance     = g_hInst;
    wc.lpszClassName = _T("SplashScreenClass");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClass(&wc))
        return 1;

    // ���� Splash ����
    g_hSplashWnd = CreateWindow(
        wc.lpszClassName,
        _T("Splash Screen"),
        WS_OVERLAPPED | WS_CAPTION,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        nullptr, nullptr, g_hInst, nullptr);
    if (!g_hSplashWnd)
        return 1;

    ShowWindow(g_hSplashWnd, SW_SHOW);
    UpdateWindow(g_hSplashWnd);

    // ������Ϣѭ��
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

int main()
{
    // ���� Splash Screen �߳�
    HANDLE hSplashThread = CreateThread(nullptr, 0, SplashScreenThread, nullptr, 0, nullptr);
    if (!hSplashThread)
        return -1;

    // ���߳�ģ����ع��̣����� 5 ���ڼ�����ɣ�
    for (int i = 0; i <= 100; ++i)
    {
        g_progress = i / 100.0f;
        // ���� Splash �����ػ�
        if (g_hSplashWnd) InvalidateRect(g_hSplashWnd, nullptr, TRUE);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // ������ɺ󣬷��͹ر���Ϣ�� Splash ����
    if (g_hSplashWnd)
        PostMessage(g_hSplashWnd, WM_CLOSE, 0, 0);

    // �ȴ� Splash �߳̽���
    WaitForSingleObject(hSplashThread, INFINITE);
    CloseHandle(hSplashThread);

    // ��ʼ�� GLFW ������Ӧ�ô���
    if (!glfwInit())
        return -1;

    GLFWwindow* mainWindow = glfwCreateWindow(800, 600, "Main Application", nullptr, nullptr);
    if (!mainWindow)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(mainWindow);
    glfwSwapInterval(1);

    // ��������Ϣѭ����ʹ�� GLFW ������¼�����
    while (!glfwWindowShouldClose(mainWindow))
    {

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    return 0;
}
