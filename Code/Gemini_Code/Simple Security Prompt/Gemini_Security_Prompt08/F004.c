// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
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
} Var;

int main(void) {
    Var *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "SET") == 0) {
            char *key = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *value = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].value);
                    list[i].value = strdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Var *nb = realloc(list, cap * sizeof(Var));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].key = strdup(key);
                    list[count].value = strdup(value);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, arg1) == 0) {
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
            for (size_t i = 0; arg1[i] != '\0'; ) {
                if (arg1[i] == '{') {
                    size_t j = i + 1;
                    while (arg1[j] != '\0' && arg1[j] != '}') {
                        j++;
                    }
                    if (arg1[j] == '}') {
                        size_t klen = j - i - 1;
                        char *key = malloc(klen + 1);
                        if (key) {
                            memcpy(key, arg1 + i + 1, klen);
                            key[klen] = '\0';
                            char *val = NULL;
                            for (size_t v = 0; v < count; v++) {
                                if (strcmp(list[v].key, key) == 0) {
                                    val = list[v].value;
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
                            free(key);
                        } else {
                            putchar('{');
                            i++;
                        }
                    } else {
                        putchar('{');
                        i++;
                    }
                } else {
                    putchar(arg1[i]);
                    i++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].key, list[i].value);
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
