// F004.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

int main(void) {
    Variable *vars = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *value = p;
            if (*key == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }
            if (found_idx != -1) {
                char *v = strdup(value);
                if (v) {
                    free(vars[found_idx].value);
                    vars[found_idx].value = v;
                }
            } else {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Variable *new_vars = realloc(vars, new_cap * sizeof(Variable));
                    if (!new_vars) continue;
                    vars = new_vars;
                    capacity = new_cap;
                }
                char *k = strdup(key);
                char *v = strdup(value);
                if (k && v) {
                    vars[count].key = k;
                    vars[count].value = v;
                    count++;
                } else {
                    free(k);
                    free(v);
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*key == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        vars[j] = vars[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            char *ptr = text;
            while (*ptr) {
                if (*ptr == '{') {
                    char *end = strchr(ptr, '}');
                    if (end) {
                        size_t klen = end - (ptr + 1);
                        char *k = malloc(klen + 1);
                        if (k) {
                            memcpy(k, ptr + 1, klen);
                            k[klen] = '\0';
                            int found = -1;
                            for (size_t i = 0; i < count; i++) {
                                if (strcmp(vars[i].key, k) == 0) {
                                    found = (int)i;
                                    break;
                                }
                            }
                            if (found != -1) {
                                printf("%s", vars[found].value);
                                ptr = end + 1;
                            } else {
                                putchar('{');
                                ptr++;
                            }
                            free(k);
                        } else {
                            putchar('{');
                            ptr++;
                        }
                    } else {
                        putchar('{');
                        ptr++;
                    }
                } else {
                    putchar(*ptr);
                    ptr++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    free(line);
    return 0;
}
