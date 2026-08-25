// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_TITLE 100
#define MAX_NOTE 1000

typedef struct {
    char title[MAX_TITLE];
    char note[MAX_NOTE];
    int done;
} Task;

typedef struct {
    char name[MAX_NAME];
    Task tasks[1000];
    int task_count;
} Project;

Project projects[1000];
int proj_count = 0;

int find_proj(const char *name) {
    for (int i = 0; i < proj_count; i++) if (strcmp(projects[i].name, name) == 0) return i;
    return -1;
}

int find_task(Project *p, const char *title) {
    for (int i = 0; i < p->task_count; i++) if (strcmp(p->tasks[i].title, title) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], title[MAX_TITLE], note[MAX_NOTE], from[MAX_NAME], to[MAX_NAME];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "PROJECT") == 0) {
            scanf("%s", name);
            strcpy(projects[proj_count].name, name);
            projects[proj_count].task_count = 0;
            proj_count++;
        } else if (strcmp(cmd, "TASK") == 0) {
            scanf("%s %s", name, title);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            int idx = find_proj(name);
            if (idx != -1) {
                strcpy(projects[idx].tasks[projects[idx].task_count].title, title);
                strcpy(projects[idx].tasks[projects[idx].task_count].note, note);
                projects[idx].tasks[projects[idx].task_count].done = 0;
                projects[idx].task_count++;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            scanf("%s %s %s", from, to, title);
            int idx_from = find_proj(from);
            int idx_to = find_proj(to);
            if (idx_from != -1 && idx_to != -1) {
                int t_idx = find_task(&projects[idx_from], title);
                if (t_idx != -1) {
                    projects[idx_to].tasks[projects[idx_to].task_count] = projects[idx_from].tasks[t_idx];
                    projects[idx_to].task_count++;
                    for (int i = t_idx; i < projects[idx_from].task_count - 1; i++) {
                        projects[idx_from].tasks[i] = projects[idx_from].tasks[i + 1];
                    }
                    projects[idx_from].task_count--;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            scanf("%s %s", name, title);
            int idx = find_proj(name);
            if (idx != -1) {
                int t_idx = find_task(&projects[idx], title);
                if (t_idx != -1) projects[idx].tasks[t_idx].done = 1;
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            scanf("%s", name);
            int idx = find_proj(name);
            if (idx != -1) {
                for (int i = idx; i < proj_count - 1; i++) projects[i] = projects[i + 1];
                proj_count--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < proj_count; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
    }
    return 0;
}