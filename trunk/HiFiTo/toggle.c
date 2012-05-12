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
#include "hifito.h"
#include <WFExt.h>

static const TCHAR regKey[] = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");

static void refreshWindow(HWND parent, TCHAR *winClass, WPARAM wparam) {
    HWND window = NULL;
    TCHAR name[128];
    while (window = FindWindowEx(parent, window, NULL, NULL)) {
        GetClassName(window, name, 128);
        if (_tcscmp(name, winClass) == 0) {
            PostMessage(window, WM_COMMAND, wparam, 0);
        } else
            refreshWindow(window, winClass, wparam);
    }
}

/* Refreshes all open Windows Explorer Windows and the desktop. */
static void refreshExplorer() {
    /* Update explorer windows */
    refreshWindow(NULL, _T("SHELLDLL_DefView"), 0x7103);
}

/* Checks or toggles a setting in the registry */
static BOOLEAN check_toggle(const _TCHAR *keyName, DWORD valueA, DWORD valueB, boolean toggle) {

    HKEY key;
    DWORD value;
    DWORD size = sizeof(value);
    
    /* Open registry key */
    checkRegOperation(
        RegOpenKey(HKEY_CURRENT_USER, regKey, &key),
        TEXT("Error opening Explorer settings registry key.")
    );
    
    /* Read old value */
    checkRegOperation(
        RegQueryValueEx(
            key,
            keyName,
            NULL,
            NULL,
            (LPBYTE) &value,
            &size
        ),
        TEXT("Error reading registry key value.")
    );
    
    if (toggle) {
        /* Set new value */
        value = (value == valueA) ? valueB : valueA;
        checkRegOperation(
            RegSetValueEx(
                key,
                keyName, 0,
                REG_DWORD,
                (const BYTE *) &value,
                size
            ),
            TEXT("Error writing registry key value.")
        );
    }
    
    /* Close registry key */
    checkRegOperation(
        RegCloseKey(key),
        TEXT("Error closing registry key. Now this is really weird.")
    );
    
    if (toggle) {
        /* The setting was toggled, so we need to refresh all Explorer windows */
        refreshExplorer();
    }
    
    return value == valueA;
}

BOOLEAN toggleHiddenFiles() {
    return check_toggle(_T("Hidden"), 1, 2, TRUE);
}

BOOLEAN toggleHiddenExtensions() {
    return check_toggle(_T("HideFileExt"), 0, 1, TRUE);
}

BOOLEAN getHiddenFiles() {
    return check_toggle(_T("Hidden"), 1, 2, FALSE);
}

BOOLEAN getHiddenExtensions() {
    return check_toggle(_T("HideFileExt"), 0, 1, FALSE);
}