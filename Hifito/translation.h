/*
 *   Hifito: Hidden file toggler - hides or show hidden files using a hotkey.
 *   Copyright (C) 2012  Micha� Le�niewski
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

#ifndef LANG_CODE
#define LANG_CODE LANG_ENGLISH
#endif

#ifndef SUBLANG_CODE
#define SUBLANG_CODE 0x01
#endif

#ifdef LANG_CODE

#if LANG_CODE==LANG_ENGLISH
#include "translation-en.h"
#elif LANG_CODE==LANG_POLISH
#include "translation-pl.h"
#else
#error LANG_CODE macro has an invalid value.
#endif

#else
#error LANG_CODE macro is not defined.
#endif

