// F010.c
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
    char **values;
    int val_count;
    int val_cap;
} KeyIndex;

int main() {
    KeyIndex *items = NULL;
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
        if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", items[i].key);
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
            char *val = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int k_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    k_idx = i;
                    break;
                }
            }
            if (k_idx == -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    KeyIndex *nb = realloc(items, cap * sizeof(KeyIndex));
                    if (nb) items = nb;
                }
                items[count].key = strdup(key);
                items[count].values = NULL;
                items[count].val_count = 0;
                items[count].val_cap = 0;
                k_idx = count;
                count++;
            }
            KeyIndex *ki = &items[k_idx];
            if (ki->val_count >= ki->val_cap) {
                ki->val_cap = ki->val_cap == 0 ? 16 : ki->val_cap * 2;
                char **nb = realloc(ki->values, ki->val_cap * sizeof(char *));
                if (nb) ki->values = nb;
            }
            ki->values[ki->val_count] = strdup(val);
            ki->val_count++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *val = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    for (int j = 0; j < items[i].val_count; j++) {
                        if (strcmp(items[i].values[j], val) == 0) {
                            free(items[i].values[j]);
                            for (int k = j; k < items[i].val_count - 1; k++) {
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
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    for (int j = 0; j < items[i].val_count; j++) {
                        free(items[i].values[j]);
                    }
                    free(items[i].values);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    for (int j = 0; j < items[i].val_count; j++) {
                        printf("%s%s", items[i].values[j], (j == items[i].val_count - 1) ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].key);
        for (int j = 0; j < items[i].val_count; j++) {
            free(items[i].values[j]);
        }
        free(items[i].values);
    }
    free(items);
    return 0;
}
