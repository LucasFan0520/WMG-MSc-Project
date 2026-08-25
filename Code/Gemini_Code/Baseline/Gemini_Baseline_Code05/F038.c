// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

RLEString *store = NULL;
int count = 0;
int capacity = 0;

int find_rle(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *p1 = line + 6;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    store = realloc(store, capacity * sizeof(RLEString));
                }
                store[count].name = strdup(p1);
                store[count].encoded = strdup(sp + 1);
                count++;
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            int idx = find_rle(name);
            if (idx != -1) {
                const char *p = store[idx].encoded;
                int valid = 1;
                while (*p && valid) {
                    if (isdigit(*p)) {
                        long long rep = 0;
                        while (*p && isdigit(*p)) {
                            rep = rep * 10 + (*p - '0');
                            if (rep > 10000) {
                                valid = 0;
                                break;
                            }
                            p++;
                        }
                        if (*p && valid) {
                            char c = *p;
                            if (c == ' ') c = '_';
                            for (int i = 0; i < rep; i++) {
                                putchar(c);
                            }
                            p++;
                        } else {
                            valid = 0;
                        }
                    } else {
                        valid = 0;
                    }
                }
                if (valid) {
                    putchar('\n');
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = find_rle(name);
            if (idx != -1) {
                free(store[idx].name);
                free(store[idx].encoded);
                for (int i = idx; i < count - 1; i++) {
                    store[i] = store[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}
