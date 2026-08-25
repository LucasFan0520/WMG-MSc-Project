// F004.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Var;

int main(void) {
    Var *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *val = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].value);
                    list[i].value = strdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Var *temp = realloc(list, (count + 1) * sizeof(Var));
                if (temp) {
                    list = temp;
                    list[count].key = strdup(key);
                    list[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
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
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *text = p;
            char *out = NULL;
            size_t out_cap = 0;
            size_t out_len = 0;
            while (*text) {
                if (*text == '{') {
                    char *end = strchr(text, '}');
                    if (end) {
                        size_t klen = end - text - 1;
                        char *k = malloc(klen + 1);
                        if (k) {
                            strncpy(k, text + 1, klen);
                            k[klen] = '\0';
                            char *v = NULL;
                            for (size_t i = 0; i < count; i++) {
                                if (strcmp(list[i].key, k) == 0) {
                                    v = list[i].value;
                                    break;
                                }
                            }
                            free(k);
                            if (v) {
                                size_t vlen = strlen(v);
                                while (out_len + vlen >= out_cap) {
                                    out_cap = out_cap == 0 ? 128 : out_cap * 2;
                                    out = realloc(out, out_cap);
                                }
                                strcpy(out + out_len, v);
                                out_len += vlen;
                                text = end + 1;
                                continue;
                            }
                        }
                    }
                }
                while (out_len + 1 >= out_cap) {
                    out_cap = out_cap == 0 ? 128 : out_cap * 2;
                    out = realloc(out, out_cap);
                }
                out[out_len++] = *text;
                text++;
            }
            if (out) {
                out[out_len] = '\0';
                printf("%s\n", out);
                free(out);
            } else {
                printf("\n");
            }
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].key, list[i].value);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    free(line);
    return 0;
}
