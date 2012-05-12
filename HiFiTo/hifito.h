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
#include <Windows.h>
#include <ShlObj.h>
#include <tchar.h>
#include "resource.h"

/* Hotkey IDs */
#define HOTKEY_ID_HIDDEN      1
#define HOTKEY_ID_EXTENSIONS  2

/* Internal Hifito messages */
#define WM_HIFITO_NEWINSTANCE  WM_USER + 0x01

/*  */
#define HIFITO_WIN_CLASS  TEXT("HifitoMessageWindowClass")
#define HIFITO_WIN_NAME   TEXT("HifitoMessageWindow")

/* Settings structure -- all these settings are saved in the registry */
typedef struct _SettingsT {
    BOOL balloonsEnabled;
    BOOL hiddenHotkeyEnabled;
    BOOL extensionsHotkeyEnabled;
    UINT hiddenHotkey;
    UINT extensionsHotkey;
    BOOL sysfilesToo;
} SettingsT;

extern SettingsT settings;

extern HWND hMainWindow;
extern HINSTANCE instance;

/* toggle.c */
BOOLEAN toggleHiddenFiles();
BOOLEAN toggleExtensions();
BOOLEAN toggleHiddenAndSystemFiles();
BOOLEAN getHiddenFiles();
BOOLEAN getHiddenExtensions();
void linkSystemWithHiddenFiles();

/* error.c */
void fatal_error(_TCHAR *message);
void warning(_TCHAR *message);
void checkRegOperation(LSTATUS status, _TCHAR *message);
void check_not_null(void *ptr, _TCHAR *message);

/* about.c */
void openAboutDlg();

/* settingsdlg.c */
void openSettingsDlg();

/* hotkey.c */
void enableHotkeys();
void disableHotkeys();

/* settings.c */
void loadSettings();
void saveSettings();

/* mainwin.c */
void createMainWinow();