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
#include <ShlObj.h>
#include "hifito.h"

static BOOLEAN dialogVisible = FALSE;

static void updateControls(HWND hwndDlg) {
    BOOL hidden = BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HIDDEN), BM_GETCHECK, 0, 0);
    BOOL ext = BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_EXTENSIONS), BM_GETCHECK, 0, 0);
    UINT hotkey_hidden = SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), HKM_GETHOTKEY, 0, 0);
    UINT hotkey_ext = SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), HKM_GETHOTKEY, 0, 0);
    BOOL no_tray_action = BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_RADIO_TRAY_NOACTION), BM_GETCHECK, 0, 0);
    BOOL ok_enabled = (!hidden || hotkey_hidden)
                      && (!ext || hotkey_ext)
                      && (!ext || !hidden || (hotkey_hidden != hotkey_ext));
                      
    EnableWindow(GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), hidden);
    EnableWindow(GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), ext);
    EnableWindow(GetDlgItem(hwndDlg, IDC_CHECKBOX_HOTKEY_BALLOONS), ext || hidden);
    EnableWindow(GetDlgItem(hwndDlg, IDC_CHECKBOX_TRAY_BALLOONS), !(no_tray_action));
    EnableWindow(GetDlgItem(hwndDlg, IDOK), ok_enabled);
}

static BOOL CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HIDDEN), BM_SETCHECK, settings.hiddenHotkeyEnabled, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_EXTENSIONS), BM_SETCHECK, settings.extensionsHotkeyEnabled, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HOTKEY_BALLOONS), BM_SETCHECK, settings.balloonsEnabled, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), HKM_SETHOTKEY, settings.hiddenHotkey, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), HKM_SETHOTKEY, settings.extensionsHotkey, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_SYSFILES), BM_SETCHECK, settings.sysfilesToo, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_RADIO_TRAY_NOACTION), BM_SETCHECK, settings.trayAction == TRAY_ACTION_NONE, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_RADIO_TRAY_HIDDEN), BM_SETCHECK, settings.trayAction == TRAY_ACTION_HIDDEN, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_RADIO_TRAY_EXTENSIONS), BM_SETCHECK, settings.trayAction == TRAY_ACTION_EXTENSIONS, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_TRAY_BALLOONS), BM_SETCHECK, settings.balloonsTray, 0);
#ifdef PORTABLE
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_AUTOSTART), BM_SETCHECK, FALSE, 0);
            EnableWindow(GetDlgItem(hwndDlg, IDC_CHECKBOX_AUTOSTART), FALSE);
#else
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_AUTOSTART), BM_SETCHECK, autostartState(), 0);
#endif
            dialogVisible = TRUE;
            disableHotkeys();
            updateControls(hwndDlg);
            
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_CHECKBOX_HIDDEN:
                case IDC_CHECKBOX_EXTENSIONS:
                case IDC_HOTKEY_HIDDEN:
                case IDC_HOTKEY_EXTENSIONS:
                case IDC_RADIO_TRAY_HIDDEN:
                case IDC_RADIO_TRAY_EXTENSIONS:
                case IDC_RADIO_TRAY_NOACTION:
                    updateControls(hwndDlg);
                    return TRUE;
                case IDOK:
                    settings.hiddenHotkey = SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), HKM_GETHOTKEY, 0, 0);
                    settings.extensionsHotkey = SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), HKM_GETHOTKEY, 0, 0);
                    settings.hiddenHotkeyEnabled =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HIDDEN), BM_GETCHECK, 0, 0)
                        && settings.hiddenHotkey;
                    settings.extensionsHotkeyEnabled =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_EXTENSIONS), BM_GETCHECK, 0, 0)
                        && settings.extensionsHotkey;
                    settings.balloonsEnabled =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HOTKEY_BALLOONS), BM_GETCHECK, 0, 0)
                        && (settings.hiddenHotkeyEnabled || settings.balloonsEnabled);
                    settings.sysfilesToo =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_SYSFILES), BM_GETCHECK, 0, 0);
                    if (BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_RADIO_TRAY_HIDDEN), BM_GETCHECK, 0, 0))
                        settings.trayAction = TRAY_ACTION_HIDDEN;
                    else if (BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_RADIO_TRAY_EXTENSIONS), BM_GETCHECK, 0, 0))
                        settings.trayAction = TRAY_ACTION_EXTENSIONS;
                    else
                        settings.trayAction = TRAY_ACTION_NONE;
                    settings.balloonsTray =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_TRAY_BALLOONS), BM_GETCHECK, 0, 0);
                    autostartEnable(BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_AUTOSTART), BM_GETCHECK, 0, 0));
                    saveSettings();
                case IDCANCEL:
                    EndDialog(hwndDlg, LOWORD(wParam));
                    return TRUE;
            }
            return FALSE;
        case WM_DESTROY:
            dialogVisible = FALSE;
            enableHotkeys();
            /* If system files visibility is linked with hidden file visibility,
                   make sure the settings are the same after settings change. */
            if (settings.sysfilesToo)
                linkSystemWithHiddenFiles();
            return TRUE;
        default:
            return FALSE;
    }
    return TRUE;
}

void openSettingsDlg() {
    if (!dialogVisible)
        DialogBox(instance,
                  MAKEINTRESOURCE(IDD_SETUP),
                  hMainWindow,
                  DlgProc);
}