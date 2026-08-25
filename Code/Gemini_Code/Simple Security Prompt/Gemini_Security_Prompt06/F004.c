// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Variable *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "VARS") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", list[i].key, list[i].value);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "SET") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = rest;
                char *val = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].value);
                    list[found].value = strdup(val);
                } else {
                    list = realloc(list, (count + 1) * sizeof(Variable));
                    list[count].key = strdup(key);
                    list[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].key);
                free(list[found].value);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Variable));
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = rest;
            int i = 0;
            while (text[i] != '\0') {
                if (text[i] == '{') {
                    char *close = strchr(text + i, '}');
                    if (close) {
                        *close = '\0';
                        char *key = text + i + 1;
                        int found = -1;
                        for (int j = 0; j < count; j++) {
                            if (strcmp(list[j].key, key) == 0) {
                                found = j;
                                break;
                            }
                        }
                        if (found != -1) {
                            printf("%s", list[found].value);
                        } else {
                            printf("{%s}", key);
                        }
                        *close = '}';
                        i = (int)(close - text) + 1;
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
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    return 0;
}