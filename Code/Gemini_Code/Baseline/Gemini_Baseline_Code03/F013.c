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
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "TODO") == 0) {
            char title[256];
            int t_len = 0;
            if (sscanf(ptr, "%255s%n", title, &t_len) > 0) {
                char *note = ptr + t_len;
                while (*note == ' ') note++;
                tasks = realloc(tasks, (count + 1) * sizeof(Task));
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(note);
                tasks[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0 || strcmp(cmd, "DONE") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        if (strcmp(cmd, "START") == 0) tasks[i].status = 1;
                        else tasks[i].status = 2;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
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
                    if (count > 0) {
                        tasks = realloc(tasks, count * sizeof(Task));
                    } else {
                        free(tasks);
                        tasks = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(tasks[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    const char *st = (tasks[found].status == 0) ? "TODO" : ((tasks[found].status == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", st, tasks[found].note);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("TODO:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 0) printf(" %s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DOING:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 1) printf(" %s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DONE:\n");
            for (int i = 0; i < count; i++) {
                if (tasks[i].status == 2) printf(" %s %s\n", tasks[i].title, tasks[i].note);
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
