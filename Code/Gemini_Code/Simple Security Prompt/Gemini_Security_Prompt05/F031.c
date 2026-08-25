// F031.c
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
    char *note;
} Record;

int main() {
    Record *arr = NULL;
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
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_s = get_token(&p);
            if (n_s) {
                int n = atoi(n_s);
                Record *temp = malloc(n * sizeof(Record));
                int valid = 1;
                for (int i = 0; i < n; i++) {
                    char *bline = read_line(stdin);
                    if (!bline) {
                        valid = 0;
                        break;
                    }
                    char *bp = bline;
                    char *name = get_token(&bp);
                    char *note = get_rest(&bp);
                    if (!name || strlen(name) == 0) {
                        valid = 0;
                        free(bline);
                    } else {
                        temp[i].name = strdup(name);
                        temp[i].note = strdup(note);
                        free(bline);
                    }
                }
                if (valid) {
                    for (int i = 0; i < n; i++) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            Record *narr = realloc(arr, cap * sizeof(Record));
                            if (narr) arr = narr;
                        }
                        arr[count] = temp[i];
                        count++;
                    }
                } else {
                    for (int i = 0; i < n; i++) {
                        if (i < count) { 
                        }
                    }
                }
                free(temp);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; ) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        free(arr[i].note);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        printf("%s %s\n", arr[i].name, arr[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
