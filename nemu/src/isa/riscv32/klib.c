#include <stdlib.h>
#include <string.h>

size_t strlen(const char *s) {
const char *p = s;
while (*p) p++;
return p - s;
}

char *strcpy(char* dst, const char* src) {
char *ret = dst;
while ((*dst++ = *src++) != '\0');
return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
char *ret = dst;
while (n > 0) {
    if (*src != '\0') {
    *dst++ = *src++;
    } else {
    *dst++ = '\0';
    }
    n--;
}
return ret;
}

char* strcat(char* dst, const char* src) {
char *ret = dst;
while (*dst) dst++;
while ((*dst++ = *src++) != '\0');
return ret;
}

int strcmp(const char* s1, const char* s2) {
while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
}
return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
while (n > 0 && *s1 && (*s1 == *s2)) {
    s1++;
    s2++;
    n--;
}
if (n == 0) return 0;
return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void* memset(void* v, int c, size_t n) {
char *p = v;
while (n-- > 0) {
    *p++ = c;
}
return v;
}

void* memcpy(void* out, const void* in, size_t n) {
char *dst = (char*)out;
const char *src = (const char*)in;
while (n-- > 0) {
    *dst++ = *src++;
}
return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
const unsigned char *p1 = (const unsigned char*)s1;
const unsigned char *p2 = (const unsigned char*)s2;
while (n-- > 0) {
    if (*p1 != *p2) {
    return *p1 - *p2;
    }
    p1++;
    p2++;
}
return 0;
}