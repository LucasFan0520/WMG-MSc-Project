// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char *value;
} Variable;

int main() {
    Variable *vars = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *key_start = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            int key_len = p - key_start;
            char *key = malloc(key_len + 1);
            memcpy(key, key_start, key_len);
            key[key_len] = '\0';
            while (*p == ' ' || *p == '\t') p++;
            char *value = p;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(vars[found].value);
                vars[found].value = strdup(value);
                free(key);
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    vars = realloc(vars, capacity * sizeof(Variable));
                }
                vars[count].key = key;
                vars[count].value = strdup(value);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(p, "%255s", key) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(vars[found].key);
                    free(vars[found].value);
                    for (int i = found; i < count - 1; i++) {
                        vars[i] = vars[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = p;
            while (*text) {
                if (*text == '{') {
                    char *end = strchr(text, '}');
                    if (end) {
                        int k_len = end - text - 1;
                        char *k_buf = malloc(k_len + 1);
                        memcpy(k_buf, text + 1, k_len);
                        k_buf[k_len] = '\0';
                        int found = -1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, k_buf) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (found != -1) {
                            printf("%s", vars[found].value);
                            text = end + 1;
                        } else {
                            printf("{");
                            text++;
                        }
                        free(k_buf);
                    } else {
                        printf("%c", *text);
                        text++;
                    }
                } else {
                    printf("%c", *text);
                    text++;
                }
            }
            printf("\n");
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
