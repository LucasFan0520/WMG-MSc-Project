// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 1000
#define MAX_LEN 1000

typedef struct {
    char title[100];
    char note[MAX_LEN];
    int status;
} Task;

Task tasks[MAX_TASKS];
int count = 0;

int find_task(const char *title) {
    for (int i = 0; i < count; i++) if (strcmp(tasks[i].title, title) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], title[100], note[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "TODO") == 0) {
            scanf("%s", title);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (find_task(title) == -1 && count < MAX_TASKS) {
                strcpy(tasks[count].title, title);
                strcpy(tasks[count].note, note);
                tasks[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            scanf("%s", title);
            int idx = find_task(title);
            if (idx != -1) tasks[idx].status = 1;
        } else if (strcmp(cmd, "DONE") == 0) {
            scanf("%s", title);
            int idx = find_task(title);
            if (idx != -1) tasks[idx].status = 2;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", title);
            int idx = find_task(title);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) tasks[i] = tasks[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx = find_task(title);
            if (idx != -1) {
                char *s = tasks[idx].status == 0 ? "todo" : (tasks[idx].status == 1 ? "doing" : "done");
                printf("%s %s\n", s, tasks[idx].note);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("todo:");
            for (int i = 0; i < count; i++) if (tasks[i].status == 0) printf(" %s", tasks[i].title);
            printf("\ndoing:");
            for (int i = 0; i < count; i++) if (tasks[i].status == 1) printf(" %s", tasks[i].title);
            printf("\ndone:");
            for (int i = 0; i < count; i++) if (tasks[i].status == 2) printf(" %s", tasks[i].title);
            printf("\n");
        }
    }
    return 0;
}