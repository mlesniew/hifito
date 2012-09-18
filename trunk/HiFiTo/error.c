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

void fatal_error(TCHAR *message) {
    MessageBoxEx(NULL, message, TEXT("Great Scott!"), MB_ICONERROR, 0);
    exit(EXIT_FAILURE);
}

void warning(TCHAR *message) {
    MessageBoxEx(NULL, message, TEXT("Whoa, this is heavy"), MB_ICONWARNING, 0);
}

void checkRegOperation(LSTATUS status, TCHAR *message) {
    if (status != ERROR_SUCCESS)
        fatal_error(message);
}

void check_not_null(void *ptr, TCHAR *message) {
    if (!ptr)
        fatal_error(message);
}