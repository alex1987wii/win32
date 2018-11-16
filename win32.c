#include <windows.h>
#include <stdlib.h>
#include <shlwapi.h>
#include <assert.h>
#include <winsock2.h>
#define DEV_TOOLS_WIDTH         700         /* Unication dev tools main window width */
#define DEV_TOOLS_HEIGHT        550  

static HWND hwndMain,hwndChild;
static HINSTANCE hInst;
TCHAR szAppName[] = "win test";
TCHAR *tabString[] = {"tab1","tab2","tab3"};
static int CreateChildWnd(void)
{
	hwndChild = CreateWindowEx(0,WC_TABCONTROL , TEXT("button"),
WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                         50, 50,
                         300, 200,
                         hwndMain, NULL, hInst, NULL);
	if(0 == hwndChild){
		// MessageBox(NULL, TEXT ("Can not create main window"), szAppName, MB_ICONERROR);
		 return -1;
	}
	TCITEM      tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1; 
	
	tie.pszText = tabString[0];
	TabCtrl_InsertItem(hwndChild,0,&tie);
	CreateWindowEx(0,"button" , TEXT("buttonchild"),
WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                         50, 50,
                         100, 50,
                         hwndChild, NULL, hInst, NULL);
	
	
	tie.pszText = tabString[1]; 
	TabCtrl_InsertItem(hwndChild,1,&tie);
	CreateWindowEx(0,"button" , TEXT("buttonchild2"),
WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                         50, 50,
                         100, 50,
                         hwndChild, NULL, hInst, NULL);
	
	//SetWindowText(hwndChild,"child");
	return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static int i = 0;
	switch(message)
	{
		case WM_TIMER:
		
		if(i == 0)
			CreateChildWnd();
		else if(i <= 100){
			SendMessage(hwndChild,PBM_SETPOS,i,0);					
			}
		else
			KillTimer(hwndMain,1);			
		++i;
		break;
		case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,int iCmdShow)
{
	MSG msg;
	int bRet;
	int screenWidth, screenHeight;	
	static WNDCLASSEX wndclass;
	/*make sure only one instance run at the same time*/
	
	/*check the files that we need is available? */
	
	
	hInst = hInstance;
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
	
    memset(&wndclass, 0, sizeof(WNDCLASSEX));
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_HAND);
    wndclass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;
    wndclass.cbSize = sizeof(WNDCLASSEX);
    if(!RegisterClassEx(&wndclass))
      return -1;
    hwndMain = CreateWindowEx(0, szAppName, TEXT(szAppName),
                         WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS,
                         (screenWidth-DEV_TOOLS_WIDTH)/2, (screenHeight-DEV_TOOLS_HEIGHT)/2,
                         DEV_TOOLS_WIDTH, DEV_TOOLS_HEIGHT + 35,
                         NULL, NULL, hInst, NULL);
	if(0 == hwndMain){
		 MessageBox(NULL, TEXT ("Can not create main window"), szAppName, MB_ICONERROR);
		 return -1;
	}	
						 
	ShowWindow(hwndMain,iCmdShow);
	UpdateWindow(hwndMain);
	
	/*Post init message*/
	
	SetTimer(hwndMain,1,1000,NULL);
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    { 
        if (bRet == -1)
        {            
            MessageBox(NULL, TEXT("GetMessage error"), TEXT(szAppName), MB_ICONERROR);
            return -1;
        }
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        }
    }
    return msg.wParam;
}
