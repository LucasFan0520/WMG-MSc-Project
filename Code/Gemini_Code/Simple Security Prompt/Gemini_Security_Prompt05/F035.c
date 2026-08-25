// F035.c
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
    char *payload;
} Job;

int main() {
    Job *arr = NULL;
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
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = get_token(&p);
            char *payload = get_rest(&p);
            if (name) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Job *narr = realloc(arr, cap * sizeof(Job));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].payload = strdup(payload);
                count++;
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *oldName = get_token(&p);
            char *newName = get_token(&p);
            if (oldName && newName) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, oldName) == 0) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            Job *narr = realloc(arr, cap * sizeof(Job));
                            if (narr) arr = narr;
                        }
                        arr[count].name = strdup(newName);
                        arr[count].payload = strdup(arr[i].payload);
                        count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        free(arr[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        printf("%s\n", arr[i].payload);
                        free(arr[i].name);
                        free(arr[i].payload);
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
        free(arr[i].payload);
    }
    free(arr);
    return 0;
}
