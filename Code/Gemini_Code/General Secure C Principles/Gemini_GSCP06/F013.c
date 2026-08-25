// F013.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    char *status;
} Task;

int main(void) {
    Task *tasks = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TODO ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *title = args;
                char *note = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Task *next = realloc(tasks, capacity * sizeof(Task));
                    if (!next) break;
                    tasks = next;
                }
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(note);
                tasks[count].status = strdup("TODO");
                count++;
            }
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "TODO") == 0) {
                    free(tasks[i].status);
                    tasks[i].status = strdup("DOING");
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "DOING") == 0) {
                    free(tasks[i].status);
                    tasks[i].status = strdup("DONE");
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    printf("%s %s\n", tasks[i].status, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
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
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
        free(tasks[i].status);
    }
    free(tasks);
    return 0;
}
