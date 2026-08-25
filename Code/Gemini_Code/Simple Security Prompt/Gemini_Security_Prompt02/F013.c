// F013.c
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
    char *title;
    char *note;
    int status;
} Task;

int main() {
    Task *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "TODO ", 5) == 0) {
            char *t_start = line + 5;
            char *space = strchr(t_start, ' ');
            char *n_start = "";
            if (space) {
                *space = '\0';
                n_start = space + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(Task));
            }
            arr[count].title = strdup(t_start);
            arr[count].note = strdup(n_start);
            arr[count].status = 0;
            count++;
        } else if (strncmp(line, "START ", 6) == 0) {
            char *t_start = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t_start) == 0 && arr[i].status == 0) {
                    arr[i].status = 1;
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *t_start = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t_start) == 0 && arr[i].status == 1) {
                    arr[i].status = 2;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *t_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t_start) == 0) {
                    free(arr[i].title);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *t_start = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, t_start) == 0) {
                    const char *st = arr[i].status == 0 ? "TODO" : (arr[i].status == 1 ? "DOING" : "DONE");
                    printf("%s %s\n", st, arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            printf("TODO:\n");
            for (size_t i = 0; i < count; i++) {
                if (arr[i].status == 0) printf("%s %s\n", arr[i].title, arr[i].note);
            }
            printf("DOING:\n");
            for (size_t i = 0; i < count; i++) {
                if (arr[i].status == 1) printf("%s %s\n", arr[i].title, arr[i].note);
            }
            printf("DONE:\n");
            for (size_t i = 0; i < count; i++) {
                if (arr[i].status == 2) printf("%s %s\n", arr[i].title, arr[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
