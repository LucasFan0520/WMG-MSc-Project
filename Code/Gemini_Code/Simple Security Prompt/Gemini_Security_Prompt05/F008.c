// F008.c
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
    int score;
    char *note;
} Student;

int main() {
    Student *arr = NULL;
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
            char *name = get_token(&p);
            char *scr_s = get_token(&p);
            char *note = get_rest(&p);
            if (name && scr_s) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Student *narr = realloc(arr, cap * sizeof(Student));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].score = atoi(scr_s);
                arr[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = get_token(&p);
            char *scr_s = get_token(&p);
            char *note = get_rest(&p);
            if (name && scr_s) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        arr[i].score = atoi(scr_s);
                        free(arr[i].note);
                        arr[i].note = strdup(note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        free(arr[i].note);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            int sum = 0;
            for (int i = 0; i < count; i++) {
                sum += arr[i].score;
            }
            printf("%d\n", count > 0 ? sum / count : 0);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", arr[i].name, arr[i].score, arr[i].note);
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
