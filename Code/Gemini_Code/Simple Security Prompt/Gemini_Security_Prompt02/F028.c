// F028.c
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
    char *stop;
    char *note;
} Stop;

int main() {
    Stop *arr = NULL;
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
            char *stop = line + 4;
            char *space = strchr(stop, ' ');
            char *note = "";
            if (space) {
                *space = '\0';
                note = space + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(Stop));
            }
            arr[count].stop = strdup(stop);
            arr[count].note = strdup(note);
            count++;
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            char *space1 = strchr(p, ' ');
            if (space1) {
                *space1 = '\0';
                int idx = atoi(p);
                if (idx >= 0 && idx <= (int)count) {
                    char *stop = space1 + 1;
                    char *space2 = strchr(stop, ' ');
                    char *note = "";
                    if (space2) {
                        *space2 = '\0';
                        note = space2 + 1;
                    }
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(Stop));
                    }
                    for (int i = (int)count; i > idx; i--) {
                        arr[i] = arr[i - 1];
                    }
                    arr[idx].stop = strdup(stop);
                    arr[idx].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                int from = atoi(p);
                int to = atoi(space + 1);
                if (from >= 0 && from < (int)count && to >= 0 && to < (int)count && from != to) {
                    Stop target = arr[from];
                    if (from < to) {
                        for (int i = from; i < to; i++) {
                            arr[i] = arr[i + 1];
                        }
                    } else {
                        for (int i = from; i > to; i--) {
                            arr[i] = arr[i - 1];
                        }
                    }
                    arr[to] = target;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && idx < (int)count) {
                free(arr[idx].stop);
                free(arr[idx].note);
                for (size_t i = idx; i < count - 1; i++) {
                    arr[i] = arr[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].stop, stop) == 0) {
                    printf("%s\n", arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", arr[i].stop, arr[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].stop);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
