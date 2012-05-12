/*
 *   HiFiTo: Hidden file toggler - hides or show hidden files using a hotkey.
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

static BOOL CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HIDDEN), BM_SETCHECK, settings.hiddenHotkeyEnabled, 0);
            EnableWindow(GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), settings.hiddenHotkeyEnabled);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_EXTENSIONS), BM_SETCHECK, settings.extensionsHotkeyEnabled, 0);
            EnableWindow(GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), settings.extensionsHotkeyEnabled);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), BM_SETCHECK, settings.balloonsEnabled, 0);
            EnableWindow(GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), settings.hiddenHotkeyEnabled || settings.extensionsHotkeyEnabled);
            SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), HKM_SETHOTKEY, settings.hiddenHotkey, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), HKM_SETHOTKEY, settings.extensionsHotkey, 0);
            dialogVisible = TRUE;
            disableHotkeys();
            return TRUE;
        case BM_CLICK:
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_CHECKBOX_HIDDEN:
                case IDC_CHECKBOX_EXTENSIONS: {
                        BOOL hidden = BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HIDDEN), BM_GETCHECK, 0, 0);
                        BOOL ext = BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_EXTENSIONS), BM_GETCHECK, 0, 0);
                        EnableWindow(GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), hidden);
                        EnableWindow(GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), ext);
                        EnableWindow(GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), ext || hidden);
                        return TRUE;
                    }
                case IDC_HOTKEY_HIDDEN:
                case IDC_HOTKEY_EXTENSIONS: {
                        UINT a = SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_HIDDEN), HKM_GETHOTKEY, 0, 0);
                        UINT b = SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY_EXTENSIONS), HKM_GETHOTKEY, 0, 0);
                        EnableWindow(GetDlgItem(hwndDlg, IDOK), !(a == b && a != 0));
                        return TRUE;
                    }
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
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), BM_GETCHECK, 0, 0)
                        && (settings.hiddenHotkeyEnabled || settings.balloonsEnabled);
                    saveSettings();
                case IDCANCEL:
                    EndDialog(hwndDlg, LOWORD(wParam));
                    return TRUE;
            }
            return FALSE;
        case WM_DESTROY:
            dialogVisible = FALSE;
            enableHotkeys();
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