// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
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

int main(void) {
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
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    if (arr[i].status == 0) printf("TODO %s %s\n", arr[i].title, arr[i].note);
                }
                for (size_t i = 0; i < count; i++) {
                    if (arr[i].status == 1) printf("DOING %s %s\n", arr[i].title, arr[i].note);
                }
                for (size_t i = 0; i < count; i++) {
                    if (arr[i].status == 2) printf("DONE %s %s\n", arr[i].title, arr[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                char *note = p2 + 1;
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Task *narr = realloc(arr, ncap * sizeof(Task));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].title = strdup(title);
                        arr[count].note = strdup(note);
                        arr[count].status = 0;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    arr[i].status = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    arr[i].status = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    const char *st = arr[i].status == 0 ? "TODO" : (arr[i].status == 1 ? "DOING" : "DONE");
                    printf("%s %s\n", st, arr[i].note);
                    break;
                }
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
