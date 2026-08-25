// F013.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { STATUS_TODO, STATUS_DOING, STATUS_DONE } TaskStatus;
typedef struct {
    char *title;
    char *note;
    TaskStatus status;
} Task;

int main(void) {
    Task *tasks = NULL;
    int count = 0;
    int capacity = 0;
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
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            char *note = "";
            if (space) {
                *space = '\0';
                note = space + 1;
                while (*note == ' ') note++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Task *tmp = realloc(tasks, new_cap * sizeof(Task));
                if (!tmp) return 1;
                tasks = tmp;
                capacity = new_cap;
            }
            char *nt = strdup(title);
            char *nn = strdup(note);
            if (!nt || !nn) return 1;
            tasks[count].title = nt;
            tasks[count].note = nn;
            tasks[count].status = STATUS_TODO;
            count++;
        } else if (strncmp(line, "START ", 6) == 0) {
            char *title = line + 6;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && tasks[i].status == STATUS_TODO) {
                    tasks[i].status = STATUS_DOING;
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0 && tasks[i].status == STATUS_DOING) {
                    tasks[i].status = STATUS_DONE;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
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
            while (*title == ' ') title++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                const char *st = tasks[found].status == STATUS_TODO ? "TODO" :
                                 tasks[found].status == STATUS_DOING ? "DOING" : "DONE";
                printf("%s %s\n", st, tasks[found].note);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            printf("TODO:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == STATUS_TODO) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DOING:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == STATUS_DOING) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DONE:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == STATUS_DONE) printf("%s %s\n", tasks[i].title, tasks[i].note);
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
