// F005.c
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

int main() {
    char **arr = NULL;
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
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = get_token(&p);
            char *text = get_rest(&p);
            if (idx_s) {
                int index = atoi(idx_s);
                if (index >= 0 && index <= count) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        char **narr = realloc(arr, cap * sizeof(char *));
                        if (narr) arr = narr;
                    }
                    for (int i = count; i > index; i--) {
                        arr[i] = arr[i - 1];
                    }
                    arr[index] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = get_rest(&p);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                char **narr = realloc(arr, cap * sizeof(char *));
                if (narr) arr = narr;
            }
            arr[count] = strdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_s = get_token(&p);
            if (idx_s) {
                int index = atoi(idx_s);
                if (index >= 0 && index < count) {
                    free(arr[index]);
                    for (int i = index; i < count - 1; i++) {
                        arr[i] = arr[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_s = get_token(&p);
            char *text = get_rest(&p);
            if (idx_s) {
                int index = atoi(idx_s);
                if (index >= 0 && index < count) {
                    free(arr[index]);
                    arr[index] = strdup(text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *t = arr[i];
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
    return 0;
}
