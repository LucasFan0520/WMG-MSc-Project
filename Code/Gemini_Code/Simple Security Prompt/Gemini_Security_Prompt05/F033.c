// F033.c
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
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main() {
    Employee *arr = NULL;
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
            char *id = get_token(&p);
            char *name = get_token(&p);
            char *dept = get_token(&p);
            char *title = get_token(&p);
            if (id && name && dept && title) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Employee *narr = realloc(arr, cap * sizeof(Employee));
                    if (narr) arr = narr;
                }
                arr[count].id = strdup(id);
                arr[count].name = strdup(name);
                arr[count].dept = strdup(dept);
                arr[count].title = strdup(title);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = get_token(&p);
            char *name = get_token(&p);
            char *dept = get_token(&p);
            char *title = get_token(&p);
            if (id && name && dept && title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        free(arr[i].name);
                        free(arr[i].dept);
                        free(arr[i].title);
                        arr[i].name = strdup(name);
                        arr[i].dept = strdup(dept);
                        arr[i].title = strdup(title);
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
                        free(arr[i].name);
                        free(arr[i].dept);
                        free(arr[i].title);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = get_token(&p);
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].id, id) == 0) {
                        printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].name);
        free(arr[i].dept);
        free(arr[i].title);
    }
    free(arr);
    return 0;
}
