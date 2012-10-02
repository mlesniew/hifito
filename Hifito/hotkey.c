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

static UINT getModifiers(UINT code) {
    UINT result = 0;
    code = HIBYTE(LOWORD(code));
    if (code & HOTKEYF_ALT)
        result |= MOD_ALT;
    if (code & HOTKEYF_CONTROL)
        result |= MOD_CONTROL;
    if (code & HOTKEYF_SHIFT)
        result |= MOD_SHIFT;
    if (code & HOTKEYF_EXT)
        result |= MOD_WIN;
    return result;
}

static UINT getVkey(UINT code) {
    return LOBYTE(LOWORD(code));
}

void enableHotkeys() {

    BOOLEAN failHidden = FALSE;
    BOOLEAN failExtensions = FALSE;
    
    if (settings.extensionsHotkeyEnabled) {
        if (!RegisterHotKey(hMainWindow, HOTKEY_ID_EXTENSIONS, getModifiers(settings.extensionsHotkey), getVkey(settings.extensionsHotkey))) {
            DWORD e = GetLastError();
            settings.extensionsHotkeyEnabled = FALSE;
            failExtensions = TRUE;
        }
    }
    
    if (settings.hiddenHotkeyEnabled) {
        if (!RegisterHotKey(hMainWindow, HOTKEY_ID_HIDDEN, getModifiers(settings.hiddenHotkey), getVkey(settings.hiddenHotkey))) {
            DWORD e = GetLastError();
            settings.extensionsHotkeyEnabled = FALSE;
            failHidden = TRUE;
        }
    }
    
    if (failHidden && failExtensions) {
        WARNING(TEXT("Couldn't register any hotkeys. Perhaps some other application already registered it. Changing the hotkey in the settings will probably fix this problem."));
    } else if (failHidden) {
        WARNING(TEXT("Couldn't register hotkey, that toggles visibility of hidden file. Perhaps some other application already registered it. Changing the hotkey in the settings will probably fix this problem."));
    } else if (failExtensions) {
        WARNING(TEXT("Couldn't register hotkey, that toggles visibility of file extensions. Perhaps some other application already registered it. Changing the hotkey in the settings will probably fix this problem."));
    }
    
}

void disableHotkeys() {
    if (settings.extensionsHotkeyEnabled) {
        UnregisterHotKey(hMainWindow, HOTKEY_ID_EXTENSIONS);
    }
    if (settings.hiddenHotkeyEnabled) {
        UnregisterHotKey(hMainWindow, HOTKEY_ID_HIDDEN);
    }
}