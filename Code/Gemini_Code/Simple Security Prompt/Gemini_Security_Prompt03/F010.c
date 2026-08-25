// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int val_count;
} SparseIndex;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    SparseIndex *list = NULL;
    int key_count = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = -1;
            for (int i = 0; i < key_count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                list = realloc(list, (key_count + 1) * sizeof(SparseIndex));
                list[key_count].key = strdup(key);
                list[key_count].values = malloc(sizeof(char *));
                list[key_count].values[0] = strdup(val);
                list[key_count].val_count = 1;
                key_count++;
            } else {
                int vc = list[found].val_count;
                list[found].values = realloc(list[found].values, (vc + 1) * sizeof(char *));
                list[found].values[vc] = strdup(val);
                list[found].val_count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < key_count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    for (int j = 0; j < list[i].val_count; j++) {
                        if (strcmp(list[i].values[j], val) == 0) {
                            free(list[i].values[j]);
                            for (int k = j; k < list[i].val_count - 1; k++) {
                                list[i].values[k] = list[i].values[k + 1];
                            }
                            list[i].val_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < key_count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    for (int j = 0; j < list[i].val_count; j++) {
                        free(list[i].values[j]);
                    }
                    free(list[i].values);
                    list[i].key = NULL;
                    list[i].values = NULL;
                    list[i].val_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < key_count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    for (int j = 0; j < list[i].val_count; j++) {
                        printf("%s\n", list[i].values[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < key_count; i++) {
                if (list[i].key) {
                    printf("%s\n", list[i].key);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < key_count; i++) {
        if (list[i].key) {
            free(list[i].key);
            for (int j = 0; j < list[i].val_count; j++) {
                free(list[i].values[j]);
            }
            free(list[i].values);
        }
    }
    free(list);
    return 0;
}
