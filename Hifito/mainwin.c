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

static void popupBalloon(TCHAR *message) {
    NOTIFYICONDATA nid;
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize              =   NOTIFYICONDATA_V2_SIZE;
    nid.hWnd                =   hMainWindow;
    nid.uID                 =   0;
    nid.uFlags              =   NIF_INFO | NIF_SHOWTIP;
    lstrcpy(nid.szInfo, message);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

static void toggleHiddenBalloon(BOOL tray) {
    BOOL balloon = (tray && settings.balloonsTray)
                   || (!tray && settings.extensionsHotkey);
                   
    if (settings.sysfilesToo) {
        BOOL state = toggleHiddenAndSystemFiles();
        if (balloon) {
            if (state)
                popupBalloon(TEXT(TEXT_BALLOON_HIDDEN_SYSTEM_VISIBLE));
            else
                popupBalloon(TEXT(TEXT_BALLOON_HIDDEN_SYSTEM_INVISIBLE));
        }
    } else {
        BOOL state = toggleHiddenFiles();
        if (balloon) {
            if (state)
                popupBalloon(TEXT(TEXT_BALLOON_HIDDEN_VISIBLE));
            else
                popupBalloon(TEXT(TEXT_BALLOON_HIDDEN_INVISIBLE));
        }
    }
}

static void toggleExtensionsBalloon(BOOL tray) {
    BOOL balloon = (tray && settings.balloonsTray)
                   || (!tray && settings.extensionsHotkey);
                   
    BOOL state = toggleExtensions();
    
    if (balloon) {
        if (state)
            popupBalloon(TEXT(TEXT_BALLOON_EXTENSIONS_VISIBLE));
        else
            popupBalloon(TEXT(TEXT_BALLOON_EXTENSIONS_INVISIBLE));
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
                nid.cbSize              =   NOTIFYICONDATA_V2_SIZE;
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
                           settings.sysfilesToo ? TEXT(TEXT_TRAY_MENU_SHOW_HIDDEN_SYSTEM) : TEXT(TEXT_TRAY_MENU_SHOW_HIDDEN));
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_TOGGLE_EXTENSIONS, TEXT(TEXT_TRAY_MENU_SHOW_EXTENSIONS));
                AppendMenu(hTrayPopup, MF_SEPARATOR, 0, NULL);
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_SETUP, TEXT(TEXT_TRAY_MENU_SETTINGS));
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_ABOUT, TEXT(TEXT_TRAY_MENU_ABOUT));
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_QUIT, TEXT(TEXT_TRAY_MENU_QUIT));
                
                ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
                nid.cbSize = NOTIFYICONDATA_V2_SIZE;
                nid.uVersion = NOTIFYICON_VERSION;
                Shell_NotifyIcon(NIM_SETVERSION, &nid);
                
                ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
                nid.cbSize              =   NOTIFYICONDATA_V2_SIZE;
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
                
                if (wParam == HOTKEY_ID_HIDDEN)
                    toggleHiddenBalloon(FALSE);
                else if (wParam == HOTKEY_ID_EXTENSIONS)
                    toggleExtensionsBalloon(FALSE);
                else
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
            } else if (lParam == WM_LBUTTONDBLCLK) {
            
                if (settings.trayAction == TRAY_ACTION_HIDDEN)
                    toggleHiddenBalloon(TRUE);
                else if (settings.trayAction == TRAY_ACTION_EXTENSIONS)
                    toggleExtensionsBalloon(TRUE);
                    
            }
            break;
            
        case WM_HIFITO_NEWINSTANCE:
            popupBalloon(TEXT(TEXT_BALLOON_ALREADY_RUNNING));
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
    
    ASSERT_NOT_NULL(RegisterClassEx(&wc));
    
    ASSERT_NOT_NULL(CreateWindowEx(
                        WS_EX_CLIENTEDGE,
                        HIFITO_WIN_CLASS,
                        HIFITO_WIN_NAME,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                        HWND_MESSAGE, NULL, instance, NULL));
                        
}
