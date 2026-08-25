// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RleRecord;

int main(void) {
    RleRecord *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "STORE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *encoded = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int valid = 1;
            char *ep = encoded;
            while (*ep != '\0') {
                if (*ep >= '0' && *ep <= '9') {
                    char *endptr;
                    long rcount = strtol(ep, &endptr, 10);
                    if (rcount <= 0 || rcount > 100000 || endptr == ep || *endptr == '\0') {
                        valid = 0;
                        break;
                    }
                    ep = endptr + 1;
                } else {
                    valid = 0;
                    break;
                }
            }

            if (valid) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RleRecord *new_items = realloc(items, capacity * sizeof(RleRecord));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                items[count].encoded = strdup(encoded);
                if (!items[count].name || !items[count].encoded) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    char *ep = items[i].encoded;
                    while (*ep != '\0') {
                        char *endptr;
                        long rcount = strtol(ep, &endptr, 10);
                        char c = *endptr;
                        for (long j = 0; j < rcount; j++) {
                            if (c == ' ') putchar('_');
                            else putchar(c);
                        }
                        ep = endptr + 1;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].encoded);
    }
    free(items);
    free(line);
    return 0;
}
