// F038.c
#define _GNU_SOURCE
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

int find_str(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int validate_rle(const char *s) {
    if (!s || *s == '\0') return 0;
    int i = 0;
    while (s[i] != '\0') {
        if (!isdigit((unsigned char)s[i])) return 0;
        int rcount = 0;
        while (s[i] != '\0' && isdigit((unsigned char)s[i])) {
            rcount = rcount * 10 + (s[i] - '0');
            if (rcount > 2000) return 0;
            i++;
        }
        if (s[i] == '\0') return 0;
        i++;
    }
    return 1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "STORE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *enc = space2 + 1;
                    if (validate_rle(enc) && find_str(arg1) == -1) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            RLEString *tmp = realloc(store, capacity * sizeof(RLEString));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            store = tmp;
                        }
                        store[count].name = strdup(arg1);
                        store[count].encoded = strdup(enc);
                        if (store[count].name && store[count].encoded) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "DECODE") == 0) {
                int idx = find_str(arg1);
                if (idx != -1) {
                    char *s = store[idx].encoded;
                    int i = 0;
                    while (s[i] != '\0') {
                        int rcount = 0;
                        while (isdigit((unsigned char)s[i])) {
                            rcount = rcount * 10 + (s[i] - '0');
                            i++;
                        }
                        char c = s[i];
                        i++;
                        for (int k = 0; k < rcount; k++) {
                            if (c == ' ') putchar('_');
                            else putchar(c);
                        }
                    }
                    putchar('\n');
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = find_str(arg1);
                if (idx != -1) {
                    free(store[idx].name);
                    free(store[idx].encoded);
                    for (int i = idx; i < count - 1; i++) {
                        store[i] = store[i + 1];
                    }
                    count--;
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", store[i].name);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].encoded);
    }
    free(store);
    return 0;
}