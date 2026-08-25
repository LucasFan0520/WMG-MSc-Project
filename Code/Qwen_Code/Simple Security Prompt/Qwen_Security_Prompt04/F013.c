// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
} Task;

Task todo[1000], doing[1000], done[1000];
int tc = 0, dc = 0, dnc = 0;

int find_task(char *title, Task *list, int count) {
    for (int i = 0; i < count; i++) if (strcmp(list[i].title, title) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "TODO") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            char *note = rest + strlen(title);
            while (*note == ' ') note++;
            todo[tc].title = strdup(title);
            todo[tc].note = strdup(note);
            tc++;
        } else if (strcmp(cmd, "START") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            int idx = find_task(title, todo, tc);
            if (idx != -1) {
                doing[dc] = todo[idx];
                for (int i = idx; i < tc - 1; i++) todo[i] = todo[i+1];
                tc--;
                dc++;
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            int idx = find_task(title, doing, dc);
            if (idx != -1) {
                done[dnc] = doing[idx];
                for (int i = idx; i < dc - 1; i++) doing[i] = doing[i+1];
                dc--;
                dnc++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            int idx = find_task(title, todo, tc);
            if (idx != -1) {
                free(todo[idx].title); free(todo[idx].note);
                for (int i = idx; i < tc - 1; i++) todo[i] = todo[i+1];
                tc--;
            }
            idx = find_task(title, doing, dc);
            if (idx != -1) {
                free(doing[idx].title); free(doing[idx].note);
                for (int i = idx; i < dc - 1; i++) doing[i] = doing[i+1];
                dc--;
            }
            idx = find_task(title, done, dnc);
            if (idx != -1) {
                free(done[idx].title); free(done[idx].note);
                for (int i = idx; i < dnc - 1; i++) done[i] = done[i+1];
                dnc--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[1000];
            sscanf(rest, "%s", title);
            int idx = find_task(title, todo, tc);
            if (idx != -1) { printf("todo %s\n", todo[idx].note); continue; }
            idx = find_task(title, doing, dc);
            if (idx != -1) { printf("doing %s\n", doing[idx].note); continue; }
            idx = find_task(title, done, dnc);
            if (idx != -1) printf("done %s\n", done[idx].note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("todo:");
            for (int i = 0; i < tc; i++) printf(" %s", todo[i].title);
            printf("\ndoing:");
            for (int i = 0; i < dc; i++) printf(" %s", doing[i].title);
            printf("\ndone:");
            for (int i = 0; i < dnc; i++) printf(" %s", done[i].title);
            printf("\n");
        }
    }
    for (int i = 0; i < tc; i++) { free(todo[i].title); free(todo[i].note); }
    for (int i = 0; i < dc; i++) { free(doing[i].title); free(doing[i].note); }
    for (int i = 0; i < dnc; i++) { free(done[i].title); free(done[i].note); }
    return 0;
}