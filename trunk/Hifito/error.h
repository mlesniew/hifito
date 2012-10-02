#pragma once

#define FATAL_ERROR(message) {                                              \
        MessageBoxEx(NULL, message, TEXT(TEXT_ERROR_TITLE), MB_ICONERROR, 0);   \
        exit(EXIT_FAILURE);                                                     \
    }

#define WARNING(message) \
    MessageBoxEx(NULL, message, TEXT(TEXT_WARNING_TITLE), MB_ICONWARNING, 0);

#ifdef _DEBUG

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)
#define ASSERT_FAIL FATAL_ERROR(TEXT("Fail in ") TEXT(__FILE__) TEXT(" at line ") TEXT(STRINGIFY(__LINE__)))

#define ASSERT_SUCCESS(a) if ((a) != ERROR_SUCCESS) { ASSERT_FAIL; }
#define ASSERT_NOT_NULL(a) if (!(a)) { ASSERT_FAIL; }

#else

#define ASSERT_SUCCESS(a) (a)
#define ASSERT_NOT_NULL(a) (a)

#endif