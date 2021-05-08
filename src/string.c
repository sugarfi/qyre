#include <string.h>

int strcmp(char *a, char *b) {
    for (; *a; a++, b++) {
        if (*a != *b) {
            return 0;
        }
    }

    return 1;
}

int strncmp(char *a, char *b, int n) {
    int i;
    for (i = 0; i < n; i++, a++, b++) {
        if (*a != *b) {
            return 0;
        }
    }

    return 1;
}

int strlen(char *s) {
    int i;
    for (i = 0; *s; s++, i++);
    return i;
}

void memcpy(char *a, char *b, int n) {
    int i;
    for (i = 0; i < n; i++) {
        b[i] = a[i];
    }
}

void memset(char *s, char c, int n) {
    int i;
    for (i = 0; i < n; i++) {
        s[n] = c;
    }
}
