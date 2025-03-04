
#include <Windows.h>
#include <tchar.h>
#include <thread>
#include <chrono>
#include <GLFW/glfw3.h>

// 全局变量
HINSTANCE g_hInst = nullptr;
HWND g_hSplashWnd = nullptr;
volatile float g_progress = 0.0f;  // 0.0~1.0

// Splash Screen 的窗口过程，用于绘制和处理消息
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

        // 填充背景
        HBRUSH hBrushBack = (HBRUSH)GetStockObject(WHITE_BRUSH);
        FillRect(hdc, &rect, hBrushBack);

        // 绘制加载文本
        const TCHAR* loadingText = _T("Loading, please wait...");
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, loadingText, -1, &rect, DT_CENTER | DT_TOP);

        // 绘制进度条：简单用一个填充矩形模拟
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

// Splash Screen 线程入口函数
DWORD WINAPI SplashScreenThread(LPVOID lpParam)
{
    g_hInst = GetModuleHandle(NULL);

    // 注册窗口类
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc   = SplashWndProc;
    wc.hInstance     = g_hInst;
    wc.lpszClassName = _T("SplashScreenClass");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClass(&wc))
        return 1;

    // 创建 Splash 窗口
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

    // 进入消息循环
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
    // 启动 Splash Screen 线程
    HANDLE hSplashThread = CreateThread(nullptr, 0, SplashScreenThread, nullptr, 0, nullptr);
    if (!hSplashThread)
        return -1;

    // 主线程模拟加载过程（例如 5 秒内加载完成）
    for (int i = 0; i <= 100; ++i)
    {
        g_progress = i / 100.0f;
        // 请求 Splash 窗口重绘
        if (g_hSplashWnd) InvalidateRect(g_hSplashWnd, nullptr, TRUE);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // 加载完成后，发送关闭消息给 Splash 窗口
    if (g_hSplashWnd)
        PostMessage(g_hSplashWnd, WM_CLOSE, 0, 0);

    // 等待 Splash 线程结束
    WaitForSingleObject(hSplashThread, INFINITE);
    CloseHandle(hSplashThread);

    // 初始化 GLFW 创建主应用窗口
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

    // 主窗口消息循环（使用 GLFW 自身的事件处理）
    while (!glfwWindowShouldClose(mainWindow))
    {

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    return 0;
}
