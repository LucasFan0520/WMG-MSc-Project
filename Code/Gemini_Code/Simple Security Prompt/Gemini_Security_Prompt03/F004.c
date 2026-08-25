// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Var;

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
    Var *list = NULL;
    int count = 0;
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
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val = p;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    free(list[i].val);
                    list[i].val = strdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                list = realloc(list, (count + 1) * sizeof(Var));
                list[count].key = strdup(key);
                list[count].val = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].val);
                    list[i].key = NULL;
                    list[i].val = NULL;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            while (*text) {
                if (*text == '{') {
                    char *end = strchr(text, '}');
                    if (end) {
                        *end = '\0';
                        char *k = text + 1;
                        char *v = NULL;
                        for (int i = 0; i < count; i++) {
                            if (list[i].key && strcmp(list[i].key, k) == 0) {
                                v = list[i].val;
                                break;
                            }
                        }
                        if (v) {
                            printf("%s", v);
                        } else {
                            printf("{%s}", k);
                        }
                        *end = '}';
                        text = end + 1;
                    } else {
                        putchar(*text);
                        text++;
                    }
                } else {
                    putchar(*text);
                    text++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].key) {
                    printf("%s=%s\n", list[i].key, list[i].val);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].key) free(list[i].key);
        if (list[i].val) free(list[i].val);
    }
    free(list);
    return 0;
}
