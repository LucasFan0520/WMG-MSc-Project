// F013.c
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TODO ", 5) == 0) {
            char *p1 = line + 5;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    tasks = realloc(tasks, capacity * sizeof(Task));
                }
                tasks[count].title = strdup(p1);
                tasks[count].note = strdup(p2 + 1);
                tasks[count].status = 0;
                count++;
            }
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            int idx = find_task(title);
            if (idx != -1) {
                tasks[idx].status = 1;
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            int idx = find_task(title);
            if (idx != -1) {
                tasks[idx].status = 2;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            int idx = find_task(title);
            if (idx != -1) {
                free(tasks[idx].title);
                free(tasks[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    tasks[i] = tasks[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            int idx = find_task(title);
            if (idx != -1) {
                const char *st_str = tasks[idx].status == 0 ? "TODO" : (tasks[idx].status == 1 ? "DOING" : "DONE");
                printf("%s %s\n", st_str, tasks[idx].note);
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int s = 0; s < 3; s++) {
                for (int i = 0; i < count; i++) {
                    if (tasks[i].status == s) {
                        const char *st_str = s == 0 ? "TODO" : (s == 1 ? "DOING" : "DONE");
                        printf("%s %s %s\n", st_str, tasks[i].title, tasks[i].note);
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
