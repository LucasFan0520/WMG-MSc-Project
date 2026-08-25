// F026.c
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
    char *category;
    char *text;
} Note;

int main() {
    Note *arr = NULL;
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
            char *cat = get_token(&p);
            char *text = get_rest(&p);
            if (cat) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Note *narr = realloc(arr, cap * sizeof(Note));
                    if (narr) arr = narr;
                }
                arr[count].category = strdup(cat);
                arr[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = get_token(&p);
            if (cat) {
                for (int i = 0; i < count; ) {
                    if (strcmp(arr[i].category, cat) == 0) {
                        free(arr[i].category);
                        free(arr[i].text);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = get_token(&p);
            if (cat) {
                for (int i = 0; i < count; ) {
                    if (strcmp(arr[i].category, cat) != 0) {
                        free(arr[i].category);
                        free(arr[i].text);
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
            char *word = get_token(&p);
            if (word) {
                for (int i = 0; i < count; i++) {
                    if (strstr(arr[i].text, word) != NULL) {
                        printf("%s %s\n", arr[i].category, arr[i].text);
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].category, arr[i].text);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].category);
        free(arr[i].text);
    }
    free(arr);
    return 0;
}
