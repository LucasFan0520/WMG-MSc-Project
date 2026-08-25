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

Task *tasks = NULL;
int count = 0;
int capacity = 0;

int find_task(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tasks[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "TODO") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *note = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Task *tmp = realloc(tasks, capacity * sizeof(Task));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        tasks = tmp;
                    }
                    tasks[count].title = strdup(arg1);
                    tasks[count].note = strdup(note);
                    tasks[count].status = 0;
                    if (tasks[count].title && tasks[count].note) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "START") == 0) {
                int idx = find_task(arg1);
                if (idx != -1) {
                    tasks[idx].status = 1;
                }
            } else if (strcmp(cmd, "DONE") == 0) {
                int idx = find_task(arg1);
                if (idx != -1) {
                    tasks[idx].status = 2;
                }
            } else if (strcmp(cmd, "REMOVE") == 0) {
                int idx = find_task(arg1);
                if (idx != -1) {
                    free(tasks[idx].title);
                    free(tasks[idx].note);
                    for (int i = idx; i < count - 1; i++) {
                        tasks[i] = tasks[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                int idx = find_task(arg1);
                if (idx != -1) {
                    if (tasks[idx].status == 0) printf("TODO %s\n", tasks[idx].note);
                    else if (tasks[idx].status == 1) printf("DOING %s\n", tasks[idx].note);
                    else if (tasks[idx].status == 2) printf("DONE %s\n", tasks[idx].note);
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                printf("TODO:\n");
                for (int i = 0; i < count; i++) {
                    if (tasks[i].status == 0) printf("%s %s\n", tasks[i].title, tasks[i].note);
                }
                printf("DOING:\n");
                for (int i = 0; i < count; i++) {
                    if (tasks[i].status == 1) printf("%s %s\n", tasks[i].title, tasks[i].note);
                }
                printf("DONE:\n");
                for (int i = 0; i < count; i++) {
                    if (tasks[i].status == 2) printf("%s %s\n", tasks[i].title, tasks[i].note);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    return 0;
}