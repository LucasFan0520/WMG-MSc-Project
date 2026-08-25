/* F013.c */
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
    int capacity = 0;
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
            char *t = NULL;
            char *nt = NULL;
            if (space) {
                *space = '\0';
                t = strdup(args);
                nt = strdup(space + 1);
                *space = ' ';
            } else {
                t = strdup(args);
                nt = strdup("");
            }
            if (t && nt) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Task *tmp = realloc(tasks, new_cap * sizeof(Task));
                    if (tmp) {
                        tasks = tmp;
                        capacity = new_cap;
                    } else {
                        free(t);
                        free(nt);
                        continue;
                    }
                }
                tasks[count].title = t;
                tasks[count].note = nt;
                tasks[count].status = 0;
                count++;
            } else {
                free(t);
                free(nt);
            }
        } else if (strncmp(line, "START ", 6) == 0) {
            char *t = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, t) == 0 && tasks[i].status == 0) {
                    tasks[i].status = 1;
                    break;
                }
            }
        } else if (strncmp(line, "DONE ", 5) == 0) {
            char *t = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, t) == 0 && tasks[i].status == 1) {
                    tasks[i].status = 2;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *t = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, t) == 0) {
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
            char *t = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, t) == 0) {
                    const char *st_str = (tasks[i].status == 0) ? "TODO" : ((tasks[i].status == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", st_str, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int s = 0; s < 3; s++) {
                for (int i = 0; i < count; i++) {
                    if (tasks[i].status == s) {
                        const char *st_str = (s == 0) ? "TODO" : ((s == 1) ? "DOING" : "DONE");
                        printf("%s %s %s\n", st_str, tasks[i].title, tasks[i].note);
                    }
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
