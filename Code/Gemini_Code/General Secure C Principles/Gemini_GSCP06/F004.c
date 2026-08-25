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
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *key = args;
                char *value = space + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    free(vars[found].value);
                    vars[found].value = strdup(value);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Variable *next = realloc(vars, capacity * sizeof(Variable));
                        if (!next) break;
                        vars = next;
                    }
                    vars[count].key = strdup(key);
                    vars[count].value = strdup(value);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
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
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            size_t i = 0;
            while (text[i] != '\0') {
                if (text[i] == '{') {
                    size_t j = i + 1;
                    while (text[j] != '\0' && text[j] != '}') {
                        j++;
                    }
                    if (text[j] == '}') {
                        size_t k_len = j - (i + 1);
                        char *k_buf = malloc(k_len + 1);
                        if (k_buf) {
                            memcpy(k_buf, text + i + 1, k_len);
                            k_buf[k_len] = '\0';
                            char *val = NULL;
                            for (size_t v = 0; v < count; v++) {
                                if (strcmp(vars[v].key, k_buf) == 0) {
                                    val = vars[v].value;
                                    break;
                                }
                            }
                            if (val) {
                                printf("%s", val);
                                i = j + 1;
                            } else {
                                putchar('{');
                                i++;
                            }
                            free(k_buf);
                        } else {
                            putchar('{');
                            i++;
                        }
                    } else {
                        putchar('{');
                        i++;
                    }
                } else {
                    putchar(text[i]);
                    i++;
                }
            }
            putchar('\n');
        } else if (strcmp(line, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
