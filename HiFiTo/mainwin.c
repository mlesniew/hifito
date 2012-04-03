/*
 *   HiFiTo: Hidden file toggler - hides or show hidden files using a hotkey.
 *   Copyright (C) 2012  Micha� Le�niewski
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "hifito.h"
#include "resource.h"

/* Popup menu identifiers */
#define IDPM_QUIT              1
#define IDPM_TOGGLE_HIDDEN     2
#define IDPM_TOGGLE_EXTENSIONS 3
#define IDPM_ABOUT             4
#define IDPM_SETUP             5

HWND hMainWindow;
HMENU hTrayPopup;

const TCHAR g_szClassName[] = TEXT("HiFiToMessageWindowClass");

void popupBalloon(TCHAR * message) {
	if (settings.balloonsEnabled) {
		NOTIFYICONDATA nid;
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize              =   sizeof(NOTIFYICONDATA);
		nid.hWnd                =   hMainWindow;
		nid.uID                 =   0;
		nid.uFlags              =   NIF_INFO;
		lstrcpy(nid.szInfo, message);
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
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
				openAboutDlg();
				break;
			case IDPM_SETUP:
				openSettingsDlg();
				break;
			case IDPM_TOGGLE_HIDDEN:
				toggleHiddenFiles();
				break;
			case IDPM_TOGGLE_EXTENSIONS:
				toggleHiddenExtensions();
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
                NOTIFYICONDATA nid;
				hMainWindow = hwnd;
				enableHotkeys();

				hTrayPopup = CreatePopupMenu();
				AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_TOGGLE_HIDDEN, TEXT("Show hidden files"));
				AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_TOGGLE_EXTENSIONS, TEXT("Show file extensions"));
				AppendMenu(hTrayPopup, MF_SEPARATOR, 0, NULL);
				AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_SETUP, TEXT("Setup"));
				AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_ABOUT, TEXT("About"));
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_QUIT, TEXT("Quit"));
                
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
            }
            break;
        
		case WM_HOTKEY:
			EndMenu();
			if (wParam == HOTKEY_ID_HIDDEN) {
				if (toggleHiddenFiles())
					popupBalloon(TEXT("Hidden files are now visible."));
				else
					popupBalloon(TEXT("Hidden files are now invisible."));
			} else if (wParam == HOTKEY_ID_EXTENSIONS) {
				if (toggleHiddenExtensions())
					popupBalloon(TEXT("File name extensions are now visible."));
				else
					popupBalloon(TEXT("File name extensions are now invisible."));
			} else 
				return DefWindowProc(hwnd, msg, wParam, lParam);
			break;
        case WM_USER:
			if (lParam == WM_RBUTTONUP) {
                POINT p;

				CheckMenuItem(
				  hTrayPopup,
				  IDPM_TOGGLE_HIDDEN,
				  MF_BYCOMMAND | (getHiddenFiles() ? MF_CHECKED : MF_UNCHECKED)
				);
				CheckMenuItem(
				  hTrayPopup,
				  IDPM_TOGGLE_EXTENSIONS,
				  MF_BYCOMMAND | (getHiddenExtensions() ? MF_CHECKED : MF_UNCHECKED)
				);

                GetCursorPos(&p);
                SetActiveWindow(hwnd);
                TrackPopupMenuEx(hTrayPopup, TPM_LEFTALIGN, p.x, p.y, hwnd, NULL);
				PostMessage(hwnd, WM_NULL, 0, 0);
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void createMainWinow() {
    WNDCLASSEX wc;
    
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = instance;
    wc.lpszClassName = g_szClassName;
    
    check_not_null((void*) RegisterClassEx(&wc), TEXT("Couldn't register window class."));
    
    check_not_null(CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        TEXT(""),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, instance, NULL),
	TEXT("Couldn't create window."));
}
