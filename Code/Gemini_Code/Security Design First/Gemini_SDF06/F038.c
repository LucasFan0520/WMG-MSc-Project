// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RleRecord;

int main(void) {
    RleRecord *store = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "STORE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *encoded = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                RleRecord *new_store = realloc(store, new_cap * sizeof(RleRecord));
                if (!new_store) continue;
                store = new_store;
                capacity = new_cap;
            }
            char *nm = strdup(name);
            char *enc = strdup(encoded);
            if (nm && enc) {
                store[count].name = nm;
                store[count].encoded = enc;
                count++;
            } else {
                free(nm);
                free(enc);
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    char *ptr = store[i].encoded;
                    int valid = 1;
                    while (*ptr) {
                        if (*ptr >= '0' && *ptr <= '9') {
                            long cnt_val = 0;
                            while (*ptr >= '0' && *ptr <= '9') {
                                cnt_val = cnt_val * 10 + (*ptr - '0');
                                if (cnt_val > 2000) {
                                    valid = 0;
                                    break;
                                }
                                ptr++;
                            }
                            if (!valid || *ptr == '\0') {
                                valid = 0;
                                break;
                            }
                            char c = *ptr;
                            ptr++;
                            for (long k = 0; k < cnt_val; k++) {
                                putchar(c == ' ' ? '_' : c);
                            }
                        } else {
                            char c = *ptr;
                            ptr++;
                            putchar(c == ' ' ? '_' : c);
                        }
                    }
                    if (valid) {
                        putchar('\n');
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    free(line);
    return 0;
}
