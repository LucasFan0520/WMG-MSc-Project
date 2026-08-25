// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    char *status;
} Task;

int main() {
    Task *tasks = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "TODO ", 5) == 0) {
            char *p = line + 5;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                tasks = realloc(tasks, (count + 1) * sizeof(Task));
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(note);
                tasks[count].status = strdup("TODO");
                count++;
            }
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].status);
                    tasks[i].status = strdup("DOING");
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].status);
                    tasks[i].status = strdup("DONE");
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].title);
                    free(tasks[i].note);
                    free(tasks[i].status);
                    for (int j = i; j < count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }
                    count--;
                    tasks = realloc(tasks, count * sizeof(Task));
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    printf("%s %s\n", tasks[i].status, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "TODO") == 0) {
                    printf("TODO: %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "DOING") == 0) {
                    printf("DOING: %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "DONE") == 0) {
                    printf("DONE: %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
        free(tasks[i].status);
    }
    free(tasks);
    return 0;
}
