// F003.c
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
    char *name;
    char *expansion;
} Alias;

int main() {
    Alias *arr = NULL;
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
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = get_token(&p);
            char *exp = get_rest(&p);
            if (name) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].expansion);
                        arr[i].expansion = strdup(exp);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Alias *narr = realloc(arr, cap * sizeof(Alias));
                        if (narr) arr = narr;
                    }
                    arr[count].name = strdup(name);
                    arr[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        char *t = arr[i].expansion;
                        while (*t) {
                            if (*t == ' ') putchar('_');
                            else putchar(*t);
                            t++;
                        }
                        putchar('\n');
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old = get_token(&p);
            char *newname = get_token(&p);
            if (old && newname) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, old) == 0) {
                        free(arr[i].name);
                        arr[i].name = strdup(newname);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        free(arr[i].expansion);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].expansion);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].expansion);
    }
    free(arr);
    return 0;
}
