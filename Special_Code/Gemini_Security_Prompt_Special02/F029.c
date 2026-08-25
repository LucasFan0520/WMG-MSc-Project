// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *key;
    char *value;
} CleaningRecord;

int main() {
    CleaningRecord *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *key = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *val = p;
        while (*p && *p != ' ') p++;
        if (*p) *p = '\0';
        if (strcmp(cmd, "ADD") == 0) {
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(CleaningRecord));
            }
            items[count].key = mystrdup(key);
            items[count].value = mystrdup(val);
            count++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            size_t i = 0;
            while (i < count) {
                int first = 1;
                for (size_t j = 0; j < i; j++) {
                    if (strcmp(items[j].key, items[i].key) == 0) {
                        first = 0;
                        break;
                    }
                }
                if (!first) {
                    free(items[i].key);
                    free(items[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    free(items[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    printf("%s\n", items[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].key, items[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].key);
        free(items[i].value);
    }
    free(items);
    return 0;
}
