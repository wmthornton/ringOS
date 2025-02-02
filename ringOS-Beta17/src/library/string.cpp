#include "string.hpp"
#include "../memory/heap.hpp"

size_t strlen(const char *str)
{
    if (str == nullptr) return 0;
    size_t length = 0;
    while(str[length]) length++;
    return length;
}

char *strcpy(char *destination, const char *source)
{
    if (destination == nullptr) return nullptr;
    char *ptr = destination;
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

char *strncpy(char *destination, const char *source, size_t n)
{
    if (destination == nullptr) return nullptr;
    char *ptr = destination;
    for (size_t i = 0; i < n && *source != '\0'; i++)
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

char *strcat(char *destination, const char *source)
{
    if (destination == nullptr) return nullptr;
    char *ptr = destination + strlen(destination);
    while (*source != '\0') *ptr++ = *source++;
    return destination;
}

char *strchr(const char *str, char ch)
{
    if (str == nullptr || ch == 0) return nullptr;
    while (*str && *str != ch ) ++str;
    return const_cast<char*>(ch == *str ? str : nullptr);
}

int strcmp(const char *a, const char *b)
{
    if (a == nullptr || b == nullptr) return 1;
    while (*a && *a == *b) { ++a; ++b; }
    return *a - *b;
}

int strncmp(const char *a, const char *b, size_t n)
{
    if (a == nullptr || b == nullptr) return 1;
    for (size_t i = 0; i < n; i++)
    {
        if (a[i] != b[i]) return 1;
    }
    return 0;
}

char *strrm(char *str, const char *substr)
{
    if (str == nullptr || substr == nullptr) return nullptr;
    char *p, *q, *r;
    if (*substr && (q = r = strstr(str, substr)) != nullptr)
    {
        size_t len = strlen(substr);
        while ((r = strstr(p = r + len, substr)) != nullptr)
        {
            while (p < r) *q++ = *p++;
        }
        while ((*q++ = *p++) != '\0') continue;
    }
    return str;
}

static char** _strsplit(const char* s, const char* delim, size_t* nb)
{
    void *data;
    char *_s = const_cast<char*>(s);
    const char **ptrs;
    size_t ptrsSize;
    size_t nbWords = 1;
    size_t sLen = strlen(s);
    size_t delimLen = strlen(delim);

    while ((_s = strstr(_s, delim)))
    {
        _s += delimLen;
        ++nbWords;
    }
    ptrsSize = (nbWords + 1) * sizeof(char*);
    ptrs = static_cast<const char**>(malloc(ptrsSize + sLen + 1));
    data = ptrs;
    if (data)
    {
        *ptrs = _s = strcpy((static_cast<char*>(data)) + ptrsSize, s);
        if (nbWords > 1)
        {
            while (( _s = strstr(_s, delim)))
            {
                *_s = '\0';
                _s += delimLen;
                *++ptrs = _s;
            }
        }
        *++ptrs = nullptr;
    }
    if (nb) *nb = data ? nbWords : 0;
    return static_cast<char**>(data);
}

char** strsplit(const char* s, const char* delim)
{
    return _strsplit(s, delim, nullptr);
}

char** strsplit_count(const char* s, const char* delim, size_t &nb)
{
    return _strsplit(s, delim, &nb);
}

char *strstr(const char *str, const char *substr)
{
    const char *a = str, *b = substr;
    while (true)
    {
        if (!*b) return (char *)str;
        if (!*a) return nullptr;
        if (*a++ != *b++)
        {
            a = ++str;
            b = substr;
        }
    }
}

int lstrstr(const char *str, const char *substr, int skip)
{
    int count = 0;
    const char *a = str, *b = substr;
    while (true)
    {
        if (!*b)
        {
            if (skip == 0) return count;
            else skip--;
        }
        if (!*a) return -1;
        if (*a++ != *b++)
        {
            a = ++str;
            b = substr;
        }
        if (*str == '\n') count++;
    }
    return -1;
}

char *reverse(char s[])
{
    size_t c, j, i;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return s;
}