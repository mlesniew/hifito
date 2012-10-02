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

#define TEXT_OK         "OK"
#define TEXT_CANCEL     "Cancel"

/* tray menu elements */
#define TEXT_TRAY_MENU_SHOW_HIDDEN_SYSTEM   "Show hidden and system files"
#define TEXT_TRAY_MENU_SHOW_HIDDEN          "Show hidden files"
#define TEXT_TRAY_MENU_SHOW_EXTENSIONS      "Show file extensions"
#define TEXT_TRAY_MENU_SETTINGS             "Settings"
#define TEXT_TRAY_MENU_ABOUT                "About"
#define TEXT_TRAY_MENU_QUIT                 "Quit"

/* messages in balloons */
#define TEXT_BALLOON_HIDDEN_SYSTEM_VISIBLE      "Hidden and system files are now visible."
#define TEXT_BALLOON_HIDDEN_SYSTEM_INVISIBLE    "Hidden and system files are now invisible."
#define TEXT_BALLOON_HIDDEN_VISIBLE             "Hidden files are now visible."
#define TEXT_BALLOON_HIDDEN_INVISIBLE           "Hidden files are now invisible."
#define TEXT_BALLOON_EXTENSIONS_VISIBLE         "File name extensions are now visible."
#define TEXT_BALLOON_EXTENSIONS_INVISIBLE       "File name extensions are now invisible."
#define TEXT_BALLOON_ALREADY_RUNNING            "Hifito is already running."

/* settings dialog */
#define TEXT_SETTINGS_TITLE                 "Hifito settings"
#define TEXT_SETTINGS_GROUP_HOTKEYS         "Hotkeys"
#define TEXT_SETTINGS_GROUP_HOTKEYS_TEXT    "Use global hotkeys to toggle visibility of:"
#define TEXT_SETTINGS_HIDDEN_HOTKEY         "Hidden files"
#define TEXT_SETTINGS_EXTENSION_HOTKEY      "File name extensions"
#define TEXT_SETTINGS_HOTKEY                "Hotkey"
#define TEXT_SETTINGS_HOTKEY_BALLOONS       "Display a balloon after toggling"

#define TEXT_SETTINGS_GROUP_TRAY            "Tray icon"
#define TEXT_SETTINGS_GROUP_TRAY_TEXT       "Double clicking the tray icon toggles:"
#define TEXT_SETTINGS_TRAY_HIDDEN           "Hidden files"
#define TEXT_SETTINGS_TRAY_EXTENSIONS       "File name extensions"
#define TEXT_SETTINGS_TRAY_NOACTION         "None"
#define TEXT_SETTINGS_TRAY_BALLOONS         "Display a balloon after toggling"

#define TEXT_SETTINGS_GROUP_ADVANCED        "Advanced"
#define TEXT_SETTINGS_SYSTEM_TOO            "Toggle system file visibility together \nwith hidden file visibility"
#define TEXT_SETTINGS_AUTOSTART             "Start Hifito at system startup"
#define TEXT_SETTINGS_HOTKEY                "Hotkey"

/* about dialog */
#define TEXT_ABOUT_TITLE        "About Hifito..."
#define TEXT_ABOUT_TRANSLATOR   "This is the original English version."
#define TEXT_ABOUT_LICENSE      "Hifito is distributed under the \nGNU General Public License version 3.0."
#define TEXT_ABOUT_FEEDBACK     "You are welcome to send comments, feature request\nand bug reports to <mlesniew@users.sourceforge.net>"
#define TEXT_ABOUT_WEBSITE      "Visit website"
#define TEXT_ABOUT_DONATE       "Donate"

/* error messages */
#define TEXT_ERROR_AUTOSTART            "Couldn't enable application autostart."
#define TEXT_ERROR_REGISTRY             "Couldn't access the Hifito registry key."
#define TEXT_ERROR_HOTKEYS_BOTH         "Couldn't register any hotkeys. Perhaps some other application already registered it. Changing the hotkey in the settings will probably fix this problem."
#define TEXT_ERROR_HOTKEYS_HIDDEN       "Couldn't register hotkey, that toggles visibility of hidden file. Perhaps some other application already registered it. Changing the hotkey in the settings will probably fix this problem."
#define TEXT_ERROR_HOTKEYS_EXTENSIONS   "Couldn't register hotkey, that toggles visibility of file extensions. Perhaps some other application already registered it. Changing the hotkey in the settings will probably fix this problem."

#define TEXT_ERROR_TITLE                "Error"
#define TEXT_WARNING_TITLE              "Warning"
