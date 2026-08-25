// F013.c
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
    char *note;
    int status;
} Task;

int main() {
    Task *arr = NULL;
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
        if (strcmp(cmd, "TODO") == 0) {
            char *title = get_token(&p);
            char *note = get_rest(&p);
            if (title) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Task *narr = realloc(arr, cap * sizeof(Task));
                    if (narr) arr = narr;
                }
                arr[count].title = strdup(title);
                arr[count].note = strdup(note);
                arr[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0 && arr[i].status == 0) {
                        arr[i].status = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0 && arr[i].status == 1) {
                        arr[i].status = 2;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        free(arr[i].title);
                        free(arr[i].note);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        char *st = arr[i].status == 0 ? "TODO" : (arr[i].status == 1 ? "DOING" : "DONE");
                        printf("%s %s\n", st, arr[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int s = 0; s < 3; s++) {
                for (int i = 0; i < count; i++) {
                    if (arr[i].status == s) {
                        char *st = s == 0 ? "TODO" : (s == 1 ? "DOING" : "DONE");
                        printf("%s %s %s\n", st, arr[i].title, arr[i].note);
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
