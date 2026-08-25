// F038.c
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

typedef struct {
    char *name;
    char *encoded;
} RLE;

int main() {
    RLE *arr = NULL;
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
        if (strcmp(cmd, "STORE") == 0) {
            char *name = get_token(&p);
            char *enc = get_token(&p);
            if (name && enc) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    RLE *narr = realloc(arr, cap * sizeof(RLE));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].encoded = strdup(enc);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        char *curr = arr[i].encoded;
                        int ok = 1;
                        while (*curr && ok) {
                            if (*curr >= '0' && *curr <= '9') {
                                int repeat = 0;
                                while (*curr >= '0' && *curr <= '9') {
                                    repeat = repeat * 10 + (*curr - '0');
                                    curr++;
                                    if (repeat > 10000) {
                                        ok = 0;
                                        break;
                                    }
                                }
                                if (ok && *curr) {
                                    char ch = *curr;
                                    for (int k = 0; k < repeat; k++) {
                                        if (ch == ' ') putchar('_');
                                        else putchar(ch);
                                    }
                                    curr++;
                                }
                            } else {
                                if (*curr == ' ') putchar('_');
                                else putchar(*curr);
                                curr++;
                            }
                        }
                        putchar('\n');
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        free(arr[i].encoded);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].encoded);
    }
    free(arr);
    return 0;
}
