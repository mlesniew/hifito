#include <Windows.h>
#include <ShlObj.h>
#include <tchar.h>

#include "resource.h"

const TCHAR g_szClassName[] = TEXT("HiFiToMessageWindow");
const TCHAR appName[] = TEXT("HiFiTo");

#define IDPM_QUIT              1
#define IDPM_TOGGLE_HIDDEN     2
#define IDPM_TOGGLE_EXTENSIONS 3
#define IDPM_ABOUT             4
#define IDPM_SETUP             5

HINSTANCE instance;
HMENU popup;

struct SettingsStruct {
	BOOL balloonsEnabled;
	BOOL hiddenHotkeyEnabled;
	BOOL extensionsHotkeyEnabled;
} settings;

/*
 *	MessageBox title
 */
const _TCHAR dialogTitle[] = _T("HiFiTo");

/*
 *	Performs the given operation and checks result code. If the operation fails a message box is displayed and the program is
 *  aborted.
 */
#define SAFE_EXECUTE( operation )																							\
	if ( (operation) != ERROR_SUCCESS ) {																					\
		MessageBoxEx(																										\
			NULL,																											\
			_T("The following operation failed:\n ")																		\
			_T( # operation	),																								\
			dialogTitle, MB_ICONERROR, 0 );																					\
		exit(-2);																											\
	}

/*
 *	Toggles a setting in the registry.
 */
int toggle( const _TCHAR * keyName, DWORD valueA, DWORD valueB ) {

	HKEY key;
	DWORD value;
	DWORD size = sizeof( value );
			
	// Open registry key
	SAFE_EXECUTE( 
		RegOpenKey( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"), &key )
	);

	// Read old value
	SAFE_EXECUTE( 
		RegQueryValueEx(
				key,
				keyName,
				NULL,
				NULL,
				(LPBYTE) &value, 
				&size
			)
	);

	value = (value == valueA) ? valueB : valueA;

	// Set new value
	SAFE_EXECUTE( 
		RegSetValueEx(
			key, 
			keyName, 0,
			REG_DWORD,
			(const BYTE *) &value, 
			size
		)
	);

	// Close registry
	SAFE_EXECUTE( 
		RegCloseKey( key )
	);

	return value;
}

BOOLEAN toggleHiddenFiles() {
	return toggle( _T("Hidden"), 1, 2 ) == 2;
}
		
BOOLEAN toggleHiddenExtensions() {
	return toggle( _T("HideFileExt"), 0, 1 ) == 1;
}

/*
 *	Refreshes all open Windows Explorer Windows and the desktop.
 */
void refreshExplorer() {
	
	HWND window;
			
	// Update desktop
	window = FindWindow( _T("Progman"), NULL );
	PostMessage( window, WM_COMMAND, 0xA220, 0 );

	window = NULL;

	// Update explorer windows
	while (1) {
		window = FindWindowEx( NULL, window, _T("CabinetWClass"), NULL );
		if ( window != NULL )
			PostMessage( window, WM_COMMAND, 0xA220, 0 );
		else 
			break;
	}
}

BOOL CALLBACK AboutDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) { 
	if (message == WM_COMMAND && LOWORD(wParam) == IDOK) {
		EndDialog(hwndDlg, IDOK);
		return TRUE;
	} else 
		return FALSE;
} 

BOOL CALLBACK SetupDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) { 
	switch (message) {
	case WM_INITDIALOG:
		SendMessage( GetDlgItem(hwndDlg, IDC_CHECKBOX_HIDDEN), BM_SETCHECK, settings.hiddenHotkeyEnabled, NULL);
		EnableWindow( GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), settings.hiddenHotkeyEnabled );
		SendMessage( GetDlgItem(hwndDlg, IDC_CHECKBOX_EXTENSIONS), BM_SETCHECK, settings.extensionsHotkeyEnabled, NULL);
		EnableWindow( GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), settings.extensionsHotkeyEnabled );
		SendMessage( GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), BM_SETCHECK, settings.balloonsEnabled, NULL);
		EnableWindow( GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), settings.hiddenHotkeyEnabled || settings.extensionsHotkeyEnabled );
		break;
	case BM_CLICK:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) 
            { 
				case IDC_CHECKBOX_HIDDEN: 
				case IDC_CHECKBOX_EXTENSIONS:
				{
					BOOL hidden = BST_CHECKED == SendMessage( GetDlgItem(hwndDlg, IDC_CHECKBOX_HIDDEN), BM_GETCHECK, 0, 0);
					BOOL ext = BST_CHECKED == SendMessage( GetDlgItem(hwndDlg, IDC_CHECKBOX_EXTENSIONS), BM_GETCHECK, 0, 0);
					EnableWindow( GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), hidden );
					EnableWindow( GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), ext );
					EnableWindow( GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), ext || hidden );
					return TRUE;
				}
                case IDOK: 
				case IDCANCEL: 
					EndDialog(hwndDlg, LOWORD(wParam));
					return TRUE;
			}
	default:
		return FALSE;
	}
	return TRUE;
} 

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_COMMAND:
			switch (wParam) {
			case IDPM_QUIT:
				DestroyWindow(hwnd);
				break;
			case IDPM_ABOUT:
				DialogBox(instance, 
					MAKEINTRESOURCE(IDD_ABOUT), 
					hwnd, 
					AboutDialogProc);
				break;
			case IDPM_SETUP:
				DialogBox(instance, 
					MAKEINTRESOURCE(IDD_SETUP), 
					hwnd, 
					SetupDialogProc);
				break;
			}
            break;
            
        case WM_DESTROY: {
                NOTIFYICONDATA nid;
                ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
                nid.cbSize              =   sizeof(NOTIFYICONDATA);
                nid.hWnd                =   hwnd;
                nid.uID                 =   0;
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
            }
            break;
            
        case WM_CREATE: {
                UINT interval;
                NOTIFYICONDATA nid;

				popup = CreatePopupMenu();
				AppendMenu(popup, MF_ENABLED | MF_STRING, IDPM_TOGGLE_HIDDEN, TEXT("Show hidden files"));
				AppendMenu(popup, MF_ENABLED | MF_STRING, IDPM_TOGGLE_EXTENSIONS, TEXT("Show file extensions"));
				AppendMenu(popup, MF_SEPARATOR, NULL, NULL);
				AppendMenu(popup, MF_ENABLED | MF_STRING, IDPM_SETUP, TEXT("Setup"));
				AppendMenu(popup, MF_ENABLED | MF_STRING, IDPM_ABOUT, TEXT("About"));
                AppendMenu(popup, MF_ENABLED | MF_STRING, IDPM_QUIT, TEXT("Quit"));
                
                ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
                nid.cbSize = sizeof(NOTIFYICONDATA);
                nid.uVersion = NOTIFYICON_VERSION_4;
                Shell_NotifyIcon(NIM_SETVERSION, &nid);
                
                ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
                nid.cbSize              =   sizeof(NOTIFYICONDATA);
                nid.hWnd                =   hwnd;
                nid.uID                 =   0;
                nid.uFlags              =   NIF_ICON | NIF_MESSAGE;
                nid.uCallbackMessage    =   WM_USER;
                nid.hIcon               =   LoadIcon(instance, MAKEINTRESOURCE(IDI_HIFITO));
                
                Shell_NotifyIcon(NIM_ADD, &nid);
                
				if ( !RegisterHotKey(hwnd, 1, MOD_ALT | MOD_NOREPEAT, 0x48) ) {
					MessageBoxEx( NULL, _T("Error: can't register hotkey."), dialogTitle, MB_ICONERROR, 0 );
					return -1;
				}

				if (!RegisterHotKey(hwnd, 1, MOD_ALT | MOD_NOREPEAT, 0x45)) {
					MessageBoxEx( NULL, _T("Error: can't register hotkey."), dialogTitle, MB_ICONERROR, 0 );
					return -1;
				}

            }
            break;
        
		case WM_HOTKEY: {
				NOTIFYICONDATA nid;
				ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
				nid.cbSize              =   sizeof(NOTIFYICONDATA);
				nid.hWnd                =   hwnd;
				nid.uID                 =   0;
				nid.uFlags              =   NIF_INFO;

				if ( HIWORD(lParam) == 0x48 ) {
					if ( toggleHiddenFiles() ) {
						lstrcpy(nid.szInfo, TEXT("Hidden files are now visible."));
					} else {
						lstrcpy(nid.szInfo, TEXT("Hidden files are now invisible."));
					}
					Shell_NotifyIcon(NIM_MODIFY, &nid);
				} else 
					toggleHiddenExtensions();
				refreshExplorer();
			}
			break;

        case WM_USER:
            if (lParam == WM_RBUTTONUP) {
                POINT p;
                GetCursorPos(&p);
                SetActiveWindow(hwnd);
                TrackPopupMenuEx(popup, TPM_LEFTALIGN, p.x, p.y, hwnd, NULL);
            }
            
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    
    return 0;
}

HWND createWindow() {
    WNDCLASSEX wc;
    HWND hwnd;
    
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = instance;
    wc.lpszClassName = g_szClassName;
    
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL,
                   TEXT("Window Registration Failed!"),
                   appName,
                   MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }
    
    hwnd = CreateWindowEx(
               WS_EX_CLIENTEDGE,
               g_szClassName,
               appName,
               WS_OVERLAPPEDWINDOW,
               CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
               NULL, NULL, instance, NULL);
               
    if (hwnd == NULL) {
        MessageBox(NULL, TEXT("Window Creation Failed!"),
                   appName,
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    return hwnd;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow ) {
    
	MSG msg;
	instance = hInstance;

	if (strstr(lpszCmdLine, "about")) {
		MessageBoxEx( NULL, _T("Hidden File Toggle\nver 1.0\nCopyright by Micha³ Leœniewski 2011"), _T("About"), MB_ICONINFORMATION, 0 );
		return 0;
	}

	createWindow();

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
	return 0;
}