// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLEItem;

int main(void) {
    RLEItem *items = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            char *enc = "";
            if (space) {
                *space = '\0';
                enc = space + 1;
                while (*enc == ' ') enc++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                RLEItem *tmp = realloc(items, new_cap * sizeof(RLEItem));
                if (!tmp) return 1;
                items = tmp;
                capacity = new_cap;
            }
            items[count].name = strdup(name);
            items[count].encoded = strdup(enc);
            count++;
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *enc = items[found].encoded;
                int valid = 1;
                while (*enc) {
                    if (*enc >= '0' && *enc <= '9') {
                        char *endptr;
                        long rep = strtol(enc, &endptr, 10);
                        if (rep < 0 || rep > 10000 || *endptr == '\0') {
                            valid = 0;
                            break;
                        }
                        char ch = *endptr;
                        for (long r = 0; r < rep; r++) {
                            if (ch == ' ') putchar('_');
                            else putchar(ch);
                        }
                        enc = endptr + 1;
                    } else {
                        valid = 0;
                        break;
                    }
                }
                if (!valid) {
                    printf(" INVALID");
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(items[found].name);
                free(items[found].encoded);
                for (int i = found; i < count - 1; i++) {
                    items[i] = items[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].encoded);
    }
    free(items);
    return 0;
}
