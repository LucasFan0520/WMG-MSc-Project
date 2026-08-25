// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
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

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *key;
    char *value;
} Variable;

int main() {
    Variable *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *key = get_token(&p);
            char *val = get_rest(&p);
            if (key) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        free(arr[i].value);
                        arr[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Variable *narr = realloc(arr, cap * sizeof(Variable));
                        if (narr) arr = narr;
                    }
                    arr[count].key = strdup(key);
                    arr[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = get_token(&p);
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        free(arr[i].key);
                        free(arr[i].value);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = get_rest(&p);
            char *t = text;
            while (*t) {
                if (*t == '{') {
                    char *start = t + 1;
                    char *end = start;
                    while (*end && *end != '}') {
                        end++;
                    }
                    if (*end == '}') {
                        *end = '\0';
                        char *v = NULL;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(arr[i].key, start) == 0) {
                                v = arr[i].value;
                                break;
                            }
                        }
                        if (v) {
                            printf("%s", v);
                        } else {
                            printf("{%s}", start);
                        }
                        *end = '}';
                        t = end + 1;
                    } else {
                        putchar(*t);
                        t++;
                    }
                } else {
                    putchar(*t);
                    t++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].key, arr[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].key);
        free(arr[i].value);
    }
    free(arr);
    return 0;
}
