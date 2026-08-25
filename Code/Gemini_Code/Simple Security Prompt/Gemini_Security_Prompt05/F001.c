// F001.c
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
    char *id;
    char *text;
} Paragraph;

int main() {
    Paragraph *arr = NULL;
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
        if (strcmp(cmd, "NEW") == 0) {
            char *id = get_token(&p);
            char *text = get_rest(&p);
            if (id) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Paragraph *narr = realloc(arr, cap * sizeof(Paragraph));
                    if (narr) arr = narr;
                }
                arr[count].id = strdup(id);
                arr[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *id = get_token(&p);
            char *text = get_rest(&p);
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        int nlen = strlen(arr[i].text) + strlen(text) + 1;
                        char *nt = malloc(nlen);
                        if (nt) {
                            strcpy(nt, arr[i].text);
                            strcat(nt, text);
                            free(arr[i].text);
                            arr[i].text = nt;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *id = get_token(&p);
            char *text = get_rest(&p);
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        free(arr[i].text);
                        arr[i].text = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = get_token(&p);
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        free(arr[i].id);
                        free(arr[i].text);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = get_token(&p);
            if (id) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        found = 1;
                        char *t = arr[i].text;
                        while (*t) {
                            if (*t == ' ') putchar('_');
                            else putchar(*t);
                            t++;
                        }
                        putchar('\n');
                        break;
                    }
                }
                if (!found) {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].id);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].text);
    }
    free(arr);
    return 0;
}
