// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Variable *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *value = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    char *nv = strdup(value);
                    if (nv) {
                        free(list[i].value);
                        list[i].value = nv;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Variable *nl = realloc(list, capacity * sizeof(Variable));
                    if (!nl) break;
                    list = nl;
                }
                list[count].key = strdup(key);
                list[count].value = strdup(value);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            for (size_t i = 0; text[i]; ) {
                if (text[i] == '{') {
                    size_t j = i + 1;
                    while (text[j] && text[j] != '}') j++;
                    if (text[j] == '}') {
                        size_t klen = j - i - 1;
                        char *key = malloc(klen + 1);
                        if (key) {
                            memcpy(key, text + i + 1, klen);
                            key[klen] = '\0';
                            char *val = NULL;
                            for (size_t v = 0; v < count; v++) {
                                if (strcmp(list[v].key, key) == 0) {
                                    val = list[v].value;
                                    break;
                                }
                            }
                            if (val) {
                                fputs(val, stdout);
                            } else {
                                putchar('{');
                                fputs(key, stdout);
                                putchar('}');
                            }
                            free(key);
                        }
                        i = j + 1;
                    } else {
                        putchar(text[i]);
                        i++;
                    }
                } else {
                    putchar(text[i]);
                    i++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", list[i].key, list[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    return 0;
}
