// F010.c
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
    char **values;
    size_t val_count;
    size_t val_cap;
} IndexEntry;

int main() {
    IndexEntry *items = NULL;
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
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    if (items[i].val_count >= items[i].val_cap) {
                        items[i].val_cap = items[i].val_cap == 0 ? 4 : items[i].val_cap * 2;
                        items[i].values = realloc(items[i].values, items[i].val_cap * sizeof(char *));
                    }
                    items[i].values[items[i].val_count++] = mystrdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(IndexEntry));
                }
                items[count].key = mystrdup(key);
                items[count].val_count = 1;
                items[count].val_cap = 4;
                items[count].values = malloc(items[count].val_cap * sizeof(char *));
                items[count].values[0] = mystrdup(val);
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    for (size_t j = 0; j < items[i].val_count; j++) {
                        if (strcmp(items[i].values[j], val) == 0) {
                            free(items[i].values[j]);
                            for (size_t k = j; k < items[i].val_count - 1; k++) {
                                items[i].values[k] = items[i].values[k + 1];
                            }
                            items[i].val_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    for (size_t j = 0; j < items[i].val_count; j++) {
                        free(items[i].values[j]);
                    }
                    free(items[i].values);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    for (size_t j = 0; j < items[i].val_count; j++) {
                        printf("%s%s", items[i].values[j], j == items[i].val_count - 1 ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].key);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].key);
        for (size_t j = 0; j < items[i].val_count; j++) {
            free(items[i].values[j]);
        }
        free(items[i].values);
    }
    free(items);
    return 0;
}
