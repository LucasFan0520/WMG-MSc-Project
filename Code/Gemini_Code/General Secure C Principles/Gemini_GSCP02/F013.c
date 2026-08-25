// F013.c
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
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TODO ", 5) == 0) {
            char title[256];
            char note[1024];
            if (sscanf(line + 5, "%255s %[^\n]", title, note) == 2) {
                Task *tmp = realloc(tasks, (count + 1) * sizeof(Task));
                if (tmp) {
                    tasks = tmp;
                    tasks[count].title = strdup(title);
                    tasks[count].note = strdup(note);
                    tasks[count].status = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            while (*title == ' ') {
                title++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    tasks[i].status = 1;
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') {
                title++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    tasks[i].status = 2;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') {
                title++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    free(tasks[i].title);
                    free(tasks[i].note);
                    for (int j = i; j < count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') {
                title++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    const char *st = (tasks[i].status == 0) ? "TODO" : ((tasks[i].status == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", st, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            printf("TODO:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 0) {
                    printf("%s %s\n", tasks[i].title, tasks[i].note);
                }
            }
            printf("DOING:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 1) {
                    printf("%s %s\n", tasks[i].title, tasks[i].note);
                }
            }
            printf("DONE:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 2) {
                    printf("%s %s\n", tasks[i].title, tasks[i].note);
                }
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
