// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    char *status;
} Task;

int main() {
    Task *tasks = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "TODO") == 0) {
            char title[256];
            int title_bytes = 0;
            if (sscanf(args, "%255s%n", title, &title_bytes) == 1) {
                char *note = args + title_bytes;
                while (*note == ' ') note++;
                tasks = realloc(tasks, (count + 1) * sizeof(Task));
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(note);
                tasks[count].status = strdup("TODO");
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "TODO") == 0) {
                        free(tasks[i].status);
                        tasks[i].status = strdup("DOING");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0 && strcmp(tasks[i].status, "DOING") == 0) {
                        free(tasks[i].status);
                        tasks[i].status = strdup("DONE");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        free(tasks[i].title);
                        free(tasks[i].note);
                        free(tasks[i].status);
                        for (int j = i; j < count - 1; j++) {
                            tasks[j] = tasks[j + 1];
                        }
                        count--;
                        tasks = realloc(tasks, count * sizeof(Task));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        printf("%s %s\n", tasks[i].status, tasks[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", tasks[i].status, tasks[i].title, tasks[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
        free(tasks[i].status);
    }
    free(tasks);
    return 0;
}
