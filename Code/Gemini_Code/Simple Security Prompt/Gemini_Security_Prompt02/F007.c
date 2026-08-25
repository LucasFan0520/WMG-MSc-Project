// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *time;
    char *label;
    char *note;
} Act;

int cmp_act(const void *a, const void *b) {
    Act *pa = (Act *)a;
    Act *pb = (Act *)b;
    int r = strcmp(pa->time, pb->time);
    if (r != 0) return r;
    return strcmp(pa->label, pb->label);
}

int main() {
    Act *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *t_start = line + 4;
            char *s1 = strchr(t_start, ' ');
            if (s1) {
                *s1 = '\0';
                char *l_start = s1 + 1;
                char *s2 = strchr(l_start, ' ');
                char *n_start = "";
                if (s2) {
                    *s2 = '\0';
                    n_start = s2 + 1;
                }
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Act));
                }
                arr[count].time = strdup(t_start);
                arr[count].label = strdup(l_start);
                arr[count].note = strdup(n_start);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *t_start = line + 7;
            char *s1 = strchr(t_start, ' ');
            if (s1) {
                *s1 = '\0';
                char *l_start = s1 + 1;
                size_t i = 0;
                while (i < count) {
                    if (strcmp(arr[i].time, t_start) == 0 && strcmp(arr[i].label, l_start) == 0) {
                        free(arr[i].time);
                        free(arr[i].label);
                        free(arr[i].note);
                        for (size_t j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *l_start = line + 5;
            int idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].label, l_start) == 0) {
                    if (idx == -1 || strcmp(arr[i].time, arr[idx].time) < 0) {
                        idx = (int)i;
                    }
                }
            }
            if (idx != -1) {
                printf("%s %s %s\n", arr[idx].time, arr[idx].label, arr[idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            if (count > 0) {
                qsort(arr, count, sizeof(Act), cmp_act);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", arr[i].time, arr[i].label, arr[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(arr[i].time);
                free(arr[i].label);
                free(arr[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].time);
        free(arr[i].label);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
