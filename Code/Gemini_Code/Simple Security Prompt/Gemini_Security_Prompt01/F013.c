// F013.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int list_type; // 0=TODO, 1=DOING, 2=DONE
} Task;

int main() {
    Task *tasks = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "TODO") == 0 && args) {
            char *title = args;
            char *space2 = strchr(args, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            tasks = realloc(tasks, (count + 1) * sizeof(Task));
            if (!tasks) return 1;
            tasks[count].title = strdup(title);
            tasks[count].note = strdup(note);
            tasks[count].list_type = 0;
            count++;
        } else if (strcmp(cmd, "START") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, args) == 0 && tasks[i].list_type == 0) {
                    tasks[i].list_type = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, args) == 0 && tasks[i].list_type == 1) {
                    tasks[i].list_type = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, args) == 0) {
                    free(tasks[i].title);
                    free(tasks[i].note);
                    for (int j = i; j < count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }
                    count--;
                    tasks = realloc(tasks, count * sizeof(Task));
                    if (count > 0 && !tasks) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, args) == 0) {
                    const char *lname = (tasks[i].list_type == 0) ? "TODO" : ((tasks[i].list_type == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", lname, tasks[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("TODO:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].list_type == 0) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DOING:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].list_type == 1) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DONE:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].list_type == 2) printf("%s %s\n", tasks[i].title, tasks[i].note);
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
