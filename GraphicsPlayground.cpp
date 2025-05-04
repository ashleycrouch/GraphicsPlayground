// GraphicsPlayground.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GraphicsPlayground.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdio.h> 
#include <cmath>

//#define SWAPBUFFERS glXSwapBuffers(dpy, glwin)
#define BLACK_INDEX 0
#define RED_INDEX 1
#define GREEN_INDEX 2
#define BLUE_INDEX 4
#define WIDTH 300
#define HEIGHT 200

// OpenGL globals, defines, and prototypes

#define GLOBE 1
#define CYLINDER 2
#define CONE 3

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHICSPLAYGROUND, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICSPLAYGROUND));

    MSG msg;

    // Main message loop:
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

GLuint texture;
void LoadTexture() {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char data[64 * 64 * 3]; //64x64 RGB texture
    for (int i = 0; i < (64 * 64 * 3); i++) data[i] = rand() % 256;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.5f, 0.5f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glFlush();
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICSPLAYGROUND));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPHICSPLAYGROUND);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Global variables
HGLRC hRC; // OpenGL rendering context
HDC hDC;   // Device context
HWND hWnd; // Window handle

// Function to set up OpenGL
void SetupOpenGL(HWND hwnd) {
    hDC = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32, // Color depth
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24, // Depth buffer
        8,  // Stencil buffer
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
}

// Function to clean up OpenGL
void CleanupOpenGL() {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}

// Function to draw a circle
//void DrawCircle(float x, float y, float radius) {
//    glBegin(GL_LINE_LOOP);
//    for (int i = 0; i < 360; i++) {
//        float angle = i * 3.14159f / 180.0f;
//        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
//    }
//    glEnd();
//}

// Function to render the scene
//void RenderScene() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glColor3f(1.0f, 0.0f, 0.0f); // Circle color (red)
//    DrawCircle(0.0f, 0.0f, 0.5f);
//
//    SwapBuffers(hDC);
//}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hEdit;
    HWND hButton;

    switch (message)
    {
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        if (height == 0) height = 1;
        glViewport(0, 0, width, height);
        //glMatrixMode(GL_PROJECTION);
        //glMatrixMode(GL_2D);
        //glLoadIdentity();
        //gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
        //glMatrixMode(GL_MODELVIEW);
    } break;
    case WM_CREATE:
        SetupOpenGL(hWnd);
        MessageBoxA(0, (char*)glGetString(GL_VERSION), "OpenGL Version", 0);

        //glClear(GL_COLOR_BUFFER_BIT);
        ////glViewport(0, 0, 200, 200);
        ////gluPerspective(0, 0, 0, 0);
        //glBegin(GL_TRIANGLES);
        //glVertex2f(0.0f, 0.5f);
        //glVertex2f(-0.5f, -0.5f);
        //glVertex2f(0.5f, -0.5f);
        //glEnd();
        //Display();
        //SwapBuffers(hDC);
        //wglDeleteContext(hRC);
        //PostQuitMessage(0);
        //hEdit = CreateWindow(_T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        //    50, 50, 200, 25, hWnd, (HMENU)1, NULL, NULL);
        //hButton = CreateWindow(_T("BUTTON"), _T("Click here"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        //    50, 100, 100, 30, hWnd, (HMENU)1, NULL, NULL);

        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case 1: 
                //char text[100];
                //GetWindowText(hEdit, text, 100);
                //MessageBox(hWnd, text, _T("User Input"), MB_OK);
                break;
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
           // PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            glColor3f(1.0f, 0.0f, 0.0f);
            //Rectangle(hDC, 50, 50, 200, 200);
            glViewport(0, 0, 200, 200);
            glBegin(GL_TRIANGLES);
            glVertex2f(0.0f, 0.5f);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f(0.5f, -0.5f);
            glEnd();
            SwapBuffers(hDC);
            ReleaseDC(hWnd, hDC);

           // EndPaint(hWnd, &ps);
//
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
