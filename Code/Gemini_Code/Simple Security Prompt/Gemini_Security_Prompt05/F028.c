// F028.c
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
    char *stop;
    char *note;
} RouteStop;

int main() {
    RouteStop *arr = NULL;
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
            char *stop = get_token(&p);
            char *note = get_rest(&p);
            if (stop) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    RouteStop *narr = realloc(arr, cap * sizeof(RouteStop));
                    if (narr) arr = narr;
                }
                arr[count].stop = strdup(stop);
                arr[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = get_token(&p);
            char *stop = get_token(&p);
            char *note = get_rest(&p);
            if (idx_s && stop) {
                int index = atoi(idx_s);
                if (index >= 0 && index <= count) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        RouteStop *narr = realloc(arr, cap * sizeof(RouteStop));
                        if (narr) arr = narr;
                    }
                    for (int i = count; i > index; i--) {
                        arr[i] = arr[i - 1];
                    }
                    arr[index].stop = strdup(stop);
                    arr[index].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_s = get_token(&p);
            char *to_s = get_token(&p);
            if (from_s && to_s) {
                int fromIndex = atoi(from_s);
                int toIndex = atoi(to_s);
                if (fromIndex >= 0 && fromIndex < count && toIndex >= 0 && toIndex < count) {
                    RouteStop temp = arr[fromIndex];
                    if (fromIndex < toIndex) {
                        for (int i = fromIndex; i < toIndex; i++) {
                            arr[i] = arr[i + 1];
                        }
                    } else {
                        for (int i = fromIndex; i > toIndex; i--) {
                            arr[i] = arr[i - 1];
                        }
                    }
                    arr[toIndex] = temp;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_s = get_token(&p);
            if (idx_s) {
                int index = atoi(idx_s);
                if (index >= 0 && index < count) {
                    free(arr[index].stop);
                    free(arr[index].note);
                    for (int i = index; i < count - 1; i++) {
                        arr[i] = arr[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = get_token(&p);
            if (stop) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].stop, stop) == 0) {
                        printf("%s\n", arr[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].stop, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].stop);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
