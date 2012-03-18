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

/* Registry key, where HiFiTo settings are saved */
static const TCHAR regKey[] = TEXT("Software\\HiFiTo");

void loadDefaultSettings() {
	settings.balloonsEnabled = TRUE;
	settings.hiddenHotkeyEnabled = TRUE;
	settings.extensionsHotkeyEnabled = TRUE;
	settings.hiddenHotkey = 0x648;	     /* == ALT + CTRL + H */
	settings.extensionsHotkey = 0x645;   /* == ALT + CTRL + E */
	saveSettings();
}

/* Helper macros for easy reading/writing settings from/to registry */
#define loadSetting(S) \
	size = sizeof(settings.S); \
	if (RegQueryValueEx(key, TEXT(# S), NULL, NULL, (LPBYTE) &settings.S, &size) != ERROR_SUCCESS) { \
		RegCloseKey(key); \
		loadDefaultSettings(); \
		return; \
	}

#define saveSetting(S) \
	checkRegOperation(RegSetValueEx(key, TEXT(#S), 0, REG_DWORD, (LPBYTE) &settings.S, sizeof(settings.S)), \
	TEXT("Couldn't store the ") TEXT(#S) TEXT(" setting in registry."))


void loadSettings() {
	HKEY key;
	DWORD size;
			
	/* Open registry key */
	if (RegOpenKey(HKEY_CURRENT_USER, regKey, &key) != ERROR_SUCCESS) {
		loadDefaultSettings();
		return;
	}

	/* Load the settings */
	loadSetting(balloonsEnabled);
	loadSetting(hiddenHotkeyEnabled);
	loadSetting(extensionsHotkeyEnabled);
	loadSetting(hiddenHotkey);
	loadSetting(extensionsHotkey);

	/* Close registry key */
	RegCloseKey(key);
}

void saveSettings() {
	HKEY key;

	/* Open registry key */
	checkRegOperation(
		RegCreateKeyEx(HKEY_CURRENT_USER, regKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL),
		TEXT("Couldn't create registry key for HiFiTo settings.")
	);

	/* Load the settings */ 
	saveSetting(balloonsEnabled);
	saveSetting(hiddenHotkeyEnabled);
	saveSetting(extensionsHotkeyEnabled);
	saveSetting(hiddenHotkey);
	saveSetting(extensionsHotkey);

	/* Close registry key */
	RegCloseKey(key);
}