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

/* Make sure the Window uses Visual Styles */
#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "hifito.h"

HINSTANCE instance;
SettingsT settings;

BOOL isAlreadyRunning() {
	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow) {
    
	HWND window = FindWindow(HIFITO_WIN_CLASS, HIFITO_WIN_NAME);

	if ( window ) {
		/* 
		   Another Hifito instance is running, so we quit immediately.
		   Before we do that, we send a message to the running instance
		   to make it display a notification.
		*/
		PostMessage(window, WM_HIFITO_NEWINSTANCE, NULL, NULL);
	} else {
		MSG msg;
		instance = hInstance;
		
		loadSettings();
		createMainWinow();

		while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	}
	return 0;
}