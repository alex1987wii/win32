#include <windows.h>
#include <stdlib.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <assert.h>
#include <winsock2.h>
#define DEV_TOOLS_WIDTH         700         /* Unication dev tools main window width */
#define DEV_TOOLS_HEIGHT        550  

#define BUTTON_WIDTH	120
#define BUTTON_HEIGHT	40
#define BUTTON_MARGIN	20
typedef struct tagChildWnd{
	int id;
	HWND hwnd;
	int type;
	unsigned int style;
	int pos_x;
	int pos_y;
	int width;
	int height;
	TCHAR *text;
};
static HWND hwndMain,hwndInfo;
static HINSTANCE hInst;
TCHAR szAppName[] = "win test";
TCHAR *tabString[] = {"tab1","tab2","tab3"};
static TCHAR errorBuff[1024];
static int nr_child = 0;
static struct tagChildWnd childList[16];
static int add_child_wnd(TCHAR *pText )
{
	if(nr_child >= 16)
		return -1;
	childList[nr_child].id = nr_child;
	childList[nr_child].hwnd = NULL;
	//childList[nr_child].type = type;
	//childList[nr_child].style = style;
	childList[nr_child].pos_x = BUTTON_MARGIN+nr_child%4*(BUTTON_MARGIN+BUTTON_WIDTH);
	childList[nr_child].pos_y = BUTTON_MARGIN+nr_child/4*(BUTTON_HEIGHT+BUTTON_MARGIN);
	childList[nr_child].width = BUTTON_WIDTH;
	childList[nr_child].height = BUTTON_HEIGHT;
	childList[nr_child].text = pText;
	return nr_child++;
}
static inline unsigned long atoh(const char *str)
{
	if(str == NULL)
		return 0;
	unsigned long ret = 0;
	while(1)
	{
		if(isdigit(*str))
			ret += *str - '0';
		else if(isxdigit(*str)){
			ret += *str - 'A' + 10;
			if(islower(*str))
				ret -= 'a' - 'A';
		}
		/*bug:when str > 10000000*/
		else if(ret & 0xf0000000){
			break;
		}
		else{
			ret >>= 4;
			break;
		}
		ret <<= 4;
		++str;
	}
	
	return ret;
}
static int CreateChildWnd(void)
{
	/*create information window*/
	hwndInfo = CreateWindowEx(0,"edit","static information",WS_CHILD | WS_VISIBLE ,10,10,DEV_TOOLS_WIDTH - 20,
	200,hwndMain,NULL,hInst,NULL);
	if(hwndInfo == NULL){
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),NULL,errorBuff,1024,NULL);
		MessageBox(hwndMain,errorBuff,"error",MB_ICONERROR);
		exit(-1);
	}
	/*create button*/
	int i;
	for(i = 0; i < nr_child; ++i){
		childList[i].hwnd = CreateWindowEx(0,"button",childList[i].text,WS_CHILD | WS_VISIBLE ,
		childList[i].pos_x,		childList[i].pos_y + 220,childList[i].width,childList[i].height,
		hwndMain,NULL,hInst,NULL);
		if(childList[i].hwnd == NULL){
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),NULL,errorBuff,1024,NULL);
		MessageBox(hwndMain,errorBuff,"error",MB_ICONERROR);
		exit(-1);
	}
	}
	return 0;
}
BOOL CALLBACK EnableChildWindows(HWND hwnd,LPARAM bEnable)
{
	EnableWindow(hwnd,bEnable);
	return TRUE;
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static int i = 0;
	TCHAR info[256];
	switch(message)
	{
		case WM_COMMAND:
		{
			HWND hwndChild = (HWND)lParam;
			if(hwndChild == NULL)
				break;
			int j;
			for(j = 0; j < nr_child; ++j){
				if(childList[j].hwnd == hwndChild){					
					GetWindowText(hwndInfo,info,256);
					HWND hwnd = atoh(info);
					snprintf(info,256,"hwnd = %x",hwnd);
					MessageBox(hwndMain,info,"notice",MB_OK);
					switch(childList[j].id)
					{											
						case 0:
						//MessageBox(hwndMain,"button1 clicked","notice",MB_OK);
						SetForegroundWindow(hwnd);
						break;
						case 1:
						BringWindowToTop(hwnd);
						//MessageBox(hwndMain,"button2 clicked","notice",MB_OK);
						break;
						case 2:
						SetCursorPos(0,100);
						/*send WM_CHAR,'k'*/
						
						break;
						case 3:
						//MessageBox(hwndMain,"button4 clicked","notice",MB_OK);
						SendMessage(hwnd,WM_CHAR,'6',3);
						break;
						case 4:
						MoveWindow(hwnd,100,100,600,400,TRUE);
						break;
						case 5:
						{
							RECT rect;
							GetClientRect(hwnd,&rect);
							snprintf(info,256,"left:%d\tright:%d\twidth:%d\ntop:%d\tbottom:%d\theight:%d\n",
							rect.left,rect.right,rect.right-rect.left,rect.top,rect.bottom,rect.bottom-rect.top);
							SetWindowText(hwndInfo,info);
						}
						break;
					}
					break;
				}
			}			
		}
		break;
		case WM_CREATE:	
		
		break;
		case WM_TIMER:
			//EnumChildWindows(hwndMain,EnableChildWindows,(LPARAM)i);
			
			i = i == 1? 0 : 1;
		break;
		case WM_DESTROY:
		PostQuitMessage(0);
		break;
		default:
		return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,int iCmdShow)
{
	MSG msg;
	int bRet;
	int screenWidth, screenHeight;	
	static WNDCLASSEX wndclass;
	/*make sure only one instance run at the same time*/
	

	
	
	hInst = hInstance;
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
	
    memset(&wndclass, 0, sizeof(WNDCLASSEX));
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_HAND);
    wndclass.hbrBackground = (HBRUSH)(COLOR_MENU+1);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;
    wndclass.cbSize = sizeof(WNDCLASSEX);
    if(!RegisterClassEx(&wndclass))
      return -1;
    hwndMain = CreateWindowEx(0,szAppName , TEXT(szAppName),
                         WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS,
                         (screenWidth-DEV_TOOLS_WIDTH)/2, (screenHeight-DEV_TOOLS_HEIGHT)/2,
                         DEV_TOOLS_WIDTH, DEV_TOOLS_HEIGHT + 35,
                         NULL, NULL, hInst, NULL);
	if(0 == hwndMain){
		 MessageBox(NULL, TEXT ("Can not create main window"), szAppName, MB_ICONERROR);
		 return -1;
	}	
	add_child_wnd("GetChildHwnd");
	add_child_wnd("GetHwndInfo");
	add_child_wnd("SendTextTo");
	add_child_wnd("GetTextFrom");
	add_child_wnd("MoveWindowto");
	add_child_wnd("GetWindowRect");
	add_child_wnd("GetTextFrom");
	add_child_wnd("GetTextFrom");
	add_child_wnd("a");
	add_child_wnd("b");
	add_child_wnd("1");
	add_child_wnd("2");
	add_child_wnd("3");	
	CreateChildWnd();
							 
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
