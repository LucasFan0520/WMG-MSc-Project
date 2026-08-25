// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

int main() {
    Task *tasks = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "TODO") == 0) {
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (title && note) {
                tasks = realloc(tasks, (count + 1) * sizeof(Task));
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(note);
                tasks[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0 && tasks[i].status == 0) {
                        tasks[i].status = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0 && tasks[i].status == 1) {
                        tasks[i].status = 2;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        free(tasks[i].title);
                        free(tasks[i].note);
                        for (int j = i; j < count - 1; j++) {
                            tasks[j] = tasks[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(tasks);
                    tasks = NULL;
                } else {
                    tasks = realloc(tasks, count * sizeof(Task));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        char *st = "TODO";
                        if (tasks[i].status == 1) st = "DOING";
                        if (tasks[i].status == 2) st = "DONE";
                        printf("%s %s\n", st, tasks[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int s = 0; s < 3; s++) {
                char *st = "TODO";
                if (s == 1) st = "DOING";
                if (s == 2) st = "DONE";
                for (int i = 0; i < count; i++) {
                    if (tasks[i].status == s) {
                        printf("%s: %s %s\n", st, tasks[i].title, tasks[i].note);
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
    return 0;
}
