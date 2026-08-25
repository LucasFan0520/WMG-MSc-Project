// F035.c
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
    char *name;
    char *payload;
} Job;

int main() {
    Job *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char *name = line + 8;
            char *space = strchr(name, ' ');
            if (space) {
                *space = '\0';
                char *payload = space + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Job));
                }
                arr[count].name = strdup(name);
                arr[count].payload = strdup(payload);
                count++;
            }
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *old_name = line + 6;
            char *space = strchr(old_name, ' ');
            if (space) {
                *space = '\0';
                char *new_name = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, old_name) == 0) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            arr = realloc(arr, cap * sizeof(Job));
                        }
                        arr[count].name = strdup(new_name);
                        arr[count].payload = strdup(arr[i].payload);
                        count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s\n", arr[i].payload);
                    free(arr[i].name);
                    free(arr[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].payload);
    }
    free(arr);
    return 0;
}
