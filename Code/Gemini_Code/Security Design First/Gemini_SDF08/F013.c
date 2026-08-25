// F013.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

int main(void) {
    Task *tasks = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TODO ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *title = p;
                char *note = space + 1;
                Task *tmp = realloc(tasks, sizeof(Task) * (count + 1));
                if (tmp) {
                    tasks = tmp;
                    tasks[count].title = strdup(title);
                    tasks[count].note = strdup(note);
                    tasks[count].status = 0;
                    if (tasks[count].title && tasks[count].note) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    tasks[i].status = 1;
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    tasks[i].status = 2;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(tasks[found].title);
                free(tasks[found].note);
                for (int i = found; i < count - 1; i++) {
                    tasks[i] = tasks[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    const char *st = (tasks[i].status == 0) ? "TODO" : ((tasks[i].status == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", st, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int s = 0; s < 3; s++) {
                const char *st = (s == 0) ? "TODO:" : ((s == 1) ? "DOING:" : "DONE:");
                printf("%s\n", st);
                for (int i = 0; i < count; i++) {
                    if (tasks[i].status == s) {
                        printf("  %s %s\n", tasks[i].title, tasks[i].note);
                    }
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    free(line);
    return 0;
}
