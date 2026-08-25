// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

typedef struct {
    char *key;
    char *value;
} Record;

int main() {
    Record *items = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
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
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].key, items[i].value);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < count; i++) {
                for (int j = i + 1; j < count; ) {
                    if (strcmp(items[i].key, items[j].key) == 0) {
                        free(items[j].key);
                        free(items[j].value);
                        for (int k = j; k < count - 1; k++) {
                            items[k] = items[k + 1];
                        }
                        count--;
                    } else {
                        j++;
                    }
                }
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *value = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Record *nb = realloc(items, cap * sizeof(Record));
                if (nb) items = nb;
            }
            items[count].key = strdup(key);
            items[count].value = strdup(value);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; ) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    free(items[i].value);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    printf("%s\n", items[i].value);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].key);
        free(items[i].value);
    }
    free(items);
    return 0;
}
