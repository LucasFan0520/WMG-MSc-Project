// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main() {
    RLEString *store = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "STORE") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *enc = "";
            if (space2) {
                *space2 = '\0';
                enc = space2 + 1;
                while (*enc == ' ') enc++;
            }
            int valid = 1;
            char *p = enc;
            while (*p != '\0') {
                if (*p >= '0' && *p <= '9') {
                    long current_c = strtol(p, &p, 10);
                    if (current_c <= 0 || current_c > 2000 || *p == '\0') {
                        valid = 0;
                        break;
                    }
                    p++;
                } else {
                    valid = 0;
                    break;
                }
            }
            if (valid) {
                store = realloc(store, (count + 1) * sizeof(RLEString));
                if (!store) return 1;
                store[count].name = strdup(name);
                store[count].encoded = strdup(enc);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, args) == 0) {
                    char *p = store[i].encoded;
                    while (*p != '\0') {
                        long current_c = strtol(p, &p, 10);
                        char ch = *p;
                        for (int j = 0; j < current_c; j++) {
                            if (ch == ' ') {
                                putchar('_');
                            } else {
                                putchar(ch);
                            }
                        }
                        p++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, args) == 0) {
                    free(store[i].name);
                    free(store[i].encoded);
                    for (int j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    store = realloc(store, count * sizeof(RLEString));
                    if (count > 0 && !store) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", store[i].name);
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
