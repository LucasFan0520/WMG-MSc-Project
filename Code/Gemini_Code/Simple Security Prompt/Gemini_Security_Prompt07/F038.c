// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLEItem;

int main() {
    RLEItem *items = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "STORE") == 0) {
            char *name = strtok(NULL, " ");
            char *encoded = strtok(NULL, " ");
            if (name && encoded) {
                items = realloc(items, (count + 1) * sizeof(RLEItem));
                items[count].name = strdup(name);
                items[count].encoded = strdup(encoded);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        char *p = items[i].encoded;
                        int valid = 1;
                        while (*p && valid) {
                            if (isdigit((unsigned char)*p)) {
                                long cnt = 0;
                                while (*p && isdigit((unsigned char)*p)) {
                                    cnt = cnt * 10 + (*p - '0');
                                    if (cnt > 100000) {
                                        valid = 0;
                                        break;
                                    }
                                    p++;
                                }
                                if (valid && *p) {
                                    char c = *p;
                                    if (c == ' ') c = '_';
                                    for (long k = 0; k < cnt; k++) {
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
                        putchar('\n');
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        free(items[i].name);
                        free(items[i].encoded);
                        for (int j = i; j < count - 1; j++) {
                            items[j] = items[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(items);
                    items = NULL;
                } else {
                    items = realloc(items, count * sizeof(RLEItem));
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].encoded);
    }
    free(items);
    return 0;
}
