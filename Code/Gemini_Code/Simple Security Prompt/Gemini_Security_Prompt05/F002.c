// F002.c
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
    char *tag;
    char *message;
} Message;

int main() {
    Message *arr = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = get_token(&p);
            char *msg = get_rest(&p);
            if (tag) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Message *narr = realloc(arr, cap * sizeof(Message));
                    if (narr) arr = narr;
                }
                arr[count].tag = strdup(tag);
                arr[count].message = strdup(msg);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = get_token(&p);
            char *tag2 = get_token(&p);
            char *newtag = get_token(&p);
            if (tag1 && tag2 && newtag) {
                char *m1 = NULL;
                char *m2 = NULL;
                for (int i = 0; i < count; i++) {
                    if (!m1 && strcmp(arr[i].tag, tag1) == 0) m1 = arr[i].message;
                    if (!m2 && strcmp(arr[i].tag, tag2) == 0) m2 = arr[i].message;
                }
                if (m1 && m2) {
                    int nlen = strlen(m1) + 1 + strlen(m2) + 1;
                    char *nm = malloc(nlen);
                    if (nm) {
                        strcpy(nm, m1);
                        strcat(nm, " ");
                        strcat(nm, m2);
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            Message *narr = realloc(arr, cap * sizeof(Message));
                            if (narr) arr = narr;
                        }
                        arr[count].tag = strdup(newtag);
                        arr[count].message = nm;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = get_token(&p);
            if (tag) {
                for (int i = 0; i < count; ) {
                    if (strcmp(arr[i].tag, tag) == 0) {
                        free(arr[i].tag);
                        free(arr[i].message);
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
            char *tag = get_token(&p);
            if (tag) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].tag, tag) == 0) {
                        char *t = arr[i].message;
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
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].tag, arr[i].message);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].tag);
        free(arr[i].message);
    }
    free(arr);
    return 0;
}
