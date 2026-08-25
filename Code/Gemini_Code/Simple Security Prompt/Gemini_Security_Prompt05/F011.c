// F011.c
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
    char *title;
    char *body;
} Draft;

int main() {
    Draft *arr = NULL;
    int count = 0;
    int cap = 0;
    Draft *last_disc = NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = get_token(&p);
            char *body = get_rest(&p);
            if (title) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Draft *narr = realloc(arr, cap * sizeof(Draft));
                    if (narr) arr = narr;
                }
                arr[count].title = strdup(title);
                arr[count].body = strdup(body);
                count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = get_token(&p);
            char *body = get_rest(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        free(arr[i].body);
                        arr[i].body = strdup(body);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        if (last_disc) {
                            free(last_disc->title);
                            free(last_disc->body);
                            free(last_disc);
                        }
                        last_disc = malloc(sizeof(Draft));
                        if (last_disc) {
                            last_disc->title = arr[i].title;
                            last_disc->body = arr[i].body;
                        }
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (last_disc) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Draft *narr = realloc(arr, cap * sizeof(Draft));
                    if (narr) arr = narr;
                }
                arr[count].title = last_disc->title;
                arr[count].body = last_disc->body;
                count++;
                free(last_disc);
                last_disc = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        printf("%s %s\n", arr[i].title, arr[i].body);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].title, arr[i].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].body);
    }
    free(arr);
    if (last_disc) {
        free(last_disc->title);
        free(last_disc->body);
        free(last_disc);
    }
    return 0;
}
