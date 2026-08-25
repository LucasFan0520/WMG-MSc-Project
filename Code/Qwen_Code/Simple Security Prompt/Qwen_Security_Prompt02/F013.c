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

int find_title(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tasks[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[20];
    char title[1000];
    char note[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "TODO") == 0) {
            scanf("%s", title);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = 0;
            int idx = find_title(title);
            if (idx != -1) {
                tasks[idx].status = 0;
                free(tasks[idx].note);
                tasks[idx].note = strdup(note);
            } else {
                tasks = realloc(tasks, sizeof(Task) * (count + 1));
                tasks[count].title = strdup(title);
                tasks[count].note = strdup(note);
                tasks[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            scanf("%s", title);
            int idx = find_title(title);
            if (idx != -1) {
                tasks[idx].status = 1;
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            scanf("%s", title);
            int idx = find_title(title);
            if (idx != -1) {
                tasks[idx].status = 2;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", title);
            int idx = find_title(title);
            if (idx != -1) {
                free(tasks[idx].title);
                free(tasks[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    tasks[i] = tasks[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx = find_title(title);
            if (idx != -1) {
                char *s = tasks[idx].status == 0 ? "todo" : (tasks[idx].status == 1 ? "doing" : "done");
                printf("%s %s\n", s, tasks[idx].note);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            char *names[3] = {"todo", "doing", "done"};
            for (int s = 0; s < 3; s++) {
                printf("%s:", names[s]);
                for (int i = 0; i < count; i++) {
                    if (tasks[i].status == s) {
                        printf(" %s", tasks[i].title);
                    }
                }
                printf("\n");
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