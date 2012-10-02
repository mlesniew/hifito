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

/* Registry key, where Hifito settings are saved */
#ifdef PORTABLE
static const TCHAR appName[] = TEXT("Hifito");
#else
static const TCHAR regKey[] = TEXT("Software\\Hifito");
#endif

void loadDefaultSettings() {
    settings.balloonsEnabled = TRUE;
    settings.hiddenHotkeyEnabled = TRUE;
    settings.sysfilesToo = FALSE;
    settings.extensionsHotkeyEnabled = TRUE;
    settings.hiddenHotkey = 0x648;       /* == ALT + CTRL + H */
    settings.extensionsHotkey = 0x645;   /* == ALT + CTRL + E */
    settings.balloonsTray = TRUE;
    settings.trayAction = TRAY_ACTION_HIDDEN;
}

#ifdef PORTABLE

BOOL WritePrivateProfileInt(
    _In_  LPCTSTR lpAppName,
    _In_  LPCTSTR lpKeyName,
    _In_  LPINT   lpInt,
    _In_  LPCTSTR lpFileName
) {
    /* Unfortunately there is no WritePrivateProfileInt in the WinAPI, so we
       need to provide our own. To implement it we can of course use the other
       function, WritePrivateProfileString, but we first need to convert the
       integer to a string. For this we could use a sprintf-like function, but
       for some reason this makes the final binary grow by 15KB. To avoid this
       we use a quick and dirty int to string conversion. It isn't very fast,
       but it's simple and it's good enough for our task. */
    if (!lpInt) {
        return WritePrivateProfileString(lpAppName, lpKeyName, NULL, lpFileName);
    } else {
        TCHAR buf[11], *t;
        int i = *lpInt;
        int j = 10;
        
        buf[10] = TEXT('\0');
        for (j = 9; j >= 0; i /= 10, --j)
            buf[j] = TEXT('0') + (i % 10);
            
        /* skip leading zeros */
        for (t = buf; t[1] && (t[0] == TEXT('0')); ++t);
        
        return WritePrivateProfileString(lpAppName, lpKeyName, t, lpFileName);
    }
}

TCHAR *getIniPath() {
    static TCHAR lpIniFile[MAX_PATH] = { 0 };
    
    if (!*lpIniFile) {
        TCHAR *t;
        GetModuleFileName(NULL, lpIniFile, MAX_PATH);
        
        /* go to end of string */
        for (t = lpIniFile; *t; ++t);
        
        /* go back to last backslash */
        while ((t > lpIniFile) && (*t != TEXT('\\'))) --t;
        
        /* replace backslash with null */
        *t = TEXT('\0');
        
        /* append file name */
        _tcsncat(lpIniFile, TEXT("\\Hifito.ini"), MAX_PATH);
        
    } /* else: use cached value */
    
    return lpIniFile;
}

#define loadSetting(S) \
    settings.S = GetPrivateProfileInt(appName, TEXT(#S), settings.S, getIniPath());

#define saveSetting(S) \
    WritePrivateProfileInt(appName, TEXT(#S), (INT *) &settings.S, getIniPath());

#else

/* Helper macros for easy reading/writing settings from/to registry */
#define loadSetting(S) { \
        DWORD size = sizeof(settings.S); \
        RegQueryValueEx(key, TEXT(# S), NULL, NULL, (LPBYTE) &settings.S, &size); \
    }
/* Ignore errors -- if loading fails, we'll use default value loaded before. */

#define saveSetting(S) \
    ASSERT_SUCCESS(RegSetValueEx(key, TEXT(#S), 0, REG_DWORD, (LPBYTE) &settings.S, sizeof(settings.S)))
/*  Ignore errors in releases -- saving values should always be successful,
    after the registry key is opened with correct privilidges. */
#endif

void loadSettings() {
#ifndef PORTABLE
    HKEY key;
#endif
    
    /* First load default settings. */
    loadDefaultSettings();
    
#ifndef PORTABLE
    /* Open registry key */
    if (RegOpenKey(HKEY_CURRENT_USER, regKey, &key) != ERROR_SUCCESS) {
        WARNING(TEXT(TEXT_ERROR_REGISTRY));
        return;
    }
#endif
    
    /* Load the individual settings */
    loadSetting(balloonsEnabled);
    loadSetting(hiddenHotkeyEnabled);
    loadSetting(extensionsHotkeyEnabled);
    loadSetting(hiddenHotkey);
    loadSetting(extensionsHotkey);
    loadSetting(sysfilesToo);
    loadSetting(trayAction);
    loadSetting(balloonsTray);
    
#ifndef PORTABLE
    /* Close registry key */
    ASSERT_SUCCESS(RegCloseKey(key));
#endif
}

void saveSettings() {
#ifndef PORTABLE
    HKEY key;
    
    /* Open registry key */
    if (RegCreateKeyEx(HKEY_CURRENT_USER, regKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL) != ERROR_SUCCESS) {
        WARNING(TEXT(TEXT_ERROR_REGISTRY));
        return;
    }
    
#endif
    
    /* Save the settings */
    saveSetting(balloonsEnabled);
    saveSetting(hiddenHotkeyEnabled);
    saveSetting(extensionsHotkeyEnabled);
    saveSetting(hiddenHotkey);
    saveSetting(extensionsHotkey);
    saveSetting(sysfilesToo);
    saveSetting(trayAction);
    saveSetting(balloonsTray);
    
#ifndef PORTABLE
    /* Close registry key */
    ASSERT_SUCCESS(RegCloseKey(key));
#endif
    
}