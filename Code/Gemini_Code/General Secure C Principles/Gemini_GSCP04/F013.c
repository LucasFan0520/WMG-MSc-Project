// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char *note;
    char *status;
} Task;

int main(void) {
    Task *tasks = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TODO") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            if (*title != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Task *new_tasks = realloc(tasks, capacity * sizeof(Task));
                    if (new_tasks) tasks = new_tasks;
                }
                if (count < capacity) {
                    tasks[count].title = strdup(title);
                    tasks[count].note = strdup(note);
                    tasks[count].status = strdup("TODO");
                    count++;
                }
            }
        } else if (strcmp(cmd, "START") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].status);
                    tasks[i].status = strdup("DOING");
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].status);
                    tasks[i].status = strdup("DONE");
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].title);
                    free(tasks[i].note);
                    free(tasks[i].status);
                    for (size_t j = i; j < count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    printf("%s %s\n", tasks[i].status, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "TODO") == 0) {
                    printf("TODO %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "DOING") == 0) {
                    printf("DOING %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].status, "DONE") == 0) {
                    printf("DONE %s %s\n", tasks[i].title, tasks[i].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
        free(tasks[i].status);
    }
    free(tasks);
    return 0;
}
