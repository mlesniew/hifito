/*
 *   Hifito: Hidden file toggler - hides or show hidden files using a hotkey.
 *   Copyright (C) 2012  Micha³ Leœniewski
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

void popupBalloon(TCHAR *message) {
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
                    if (settings.sysfilesToo)
                        toggleHiddenAndSystemFiles();
                    else
                        toggleHiddenFiles();
                    break;
                case IDPM_TOGGLE_EXTENSIONS:
                    toggleExtensions();
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
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_TOGGLE_HIDDEN,
                           settings.sysfilesToo ? TEXT("Show hidden and system files") : TEXT("Show hidden files"));
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_TOGGLE_EXTENSIONS, TEXT("Show file extensions"));
                AppendMenu(hTrayPopup, MF_SEPARATOR, 0, NULL);
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_SETUP, TEXT("Settings"));
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
                nid.hIcon               =   LoadImage(instance, MAKEINTRESOURCE(IDI_HIFITO), IMAGE_ICON, 16, 16, LR_SHARED);
                
                /* According to WinApi documentation, Bill thinks that displaying an icon
                   in the tray menu is not a very important task. For this reason, the
                   system may sometimes refuse this kind of operation. Not because there
                   is an error, but because the system is just busy doing other important
                   stuff. This usually only occurs during system startup. Again, according
                   to the WinApi documentation, the best thing to do in such cases is
                   waiting some time and trying again. */
                while (!Shell_NotifyIcon(NIM_ADD, &nid)) {
                    /* Let's give Windows a whole second to let it finish its current job. */
                    Sleep(1000);
                }
                
                /* If system files visibility is linked with hidden file visibility,
                   make sure the settings are the same at startup. */
                if (settings.sysfilesToo)
                    linkSystemWithHiddenFiles();
                    
                /* Check and correct the autostart entry if necessary. */
                autostartUpdate();
            }
            break;
            
        case WM_HOTKEY: {
                EndMenu();
                
                if (wParam == HOTKEY_ID_HIDDEN) {
                    if (settings.sysfilesToo) {
                        if (toggleHiddenAndSystemFiles())
                            popupBalloon(TEXT("Hidden and system files are now visible."));
                        else
                            popupBalloon(TEXT("Hidden and system files are now invisible."));
                    } else {
                        if (toggleHiddenFiles())
                            popupBalloon(TEXT("Hidden files are now visible."));
                        else
                            popupBalloon(TEXT("Hidden files are now invisible."));
                    }
                } else if (wParam == HOTKEY_ID_EXTENSIONS) {
                    if (toggleExtensions())
                        popupBalloon(TEXT("File name extensions are now visible."));
                    else
                        popupBalloon(TEXT("File name extensions are now invisible."));
                } else
                    return DefWindowProc(hwnd, msg, wParam, lParam);
                break;
            }
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
                
                /*
                    The following code displays the tray menu. These magic calls
                    around TrackPopupMenuEx are really necessary. We first need to
                    bring the application window to the foreground (this is weird,
                    the window isn't visible anyway) and after the menu is displayed
                    we additionally need to post WM_NULL (the only message that
                    must be ignored by any window) to our message window. Without
                    this, the tray menu will not disappear if the user clicks
                    somewhere outside it.
                
                    Bill, why?
                */
                GetCursorPos(&p);
                SetForegroundWindow(hwnd);
                TrackPopupMenuEx(hTrayPopup, TPM_LEFTALIGN, p.x, p.y, hwnd, NULL);
                PostMessage(hwnd, WM_NULL, 0, 0);
            }
            break;
            
        case WM_HIFITO_NEWINSTANCE:
            popupBalloon(TEXT("Hifito is already running."));
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
    wc.lpszClassName = HIFITO_WIN_CLASS;
    
    check_not_null((void *) RegisterClassEx(&wc), TEXT("Couldn't register window class."));
    
    check_not_null(CreateWindowEx(
                       WS_EX_CLIENTEDGE,
                       HIFITO_WIN_CLASS,
                       HIFITO_WIN_NAME,
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                       HWND_MESSAGE, NULL, instance, NULL),
                   TEXT("Couldn't create window."));
}
