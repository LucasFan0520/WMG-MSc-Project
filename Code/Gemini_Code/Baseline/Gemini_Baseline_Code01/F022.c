// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int done;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    int task_count;
} Project;

int main() {
    Project *projects = NULL;
    int p_count = 0;
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
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                projects = realloc(projects, (p_count + 1) * sizeof(Project));
                projects[p_count].name = strdup(name);
                projects[p_count].tasks = NULL;
                projects[p_count].task_count = 0;
                p_count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char p_name[256], title[256];
            int bytes = 0;
            if (sscanf(args, "%255s %255s%n", p_name, title, &bytes) == 2) {
                char *note = args + bytes;
                while (*note == ' ') note++;
                for (int i = 0; i < p_count; i++) {
                    if (strcmp(projects[i].name, p_name) == 0) {
                        projects[i].tasks = realloc(projects[i].tasks, (projects[i].task_count + 1) * sizeof(Task));
                        projects[i].tasks[projects[i].task_count].title = strdup(title);
                        projects[i].tasks[projects[i].task_count].note = strdup(note);
                        projects[i].tasks[projects[i].task_count].done = 0;
                        projects[i].task_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            if (sscanf(args, "%255s %255s %255s", from, to, title) == 3) {
                int idx_from = -1, idx_to = -1;
                for (int i = 0; i < p_count; i++) {
                    if (strcmp(projects[i].name, from) == 0) idx_from = i;
                    if (strcmp(projects[i].name, to) == 0) idx_to = i;
                }
                if (idx_from != -1 && idx_to != -1) {
                    for (int j = 0; j < projects[idx_from].task_count; j++) {
                        if (strcmp(projects[idx_from].tasks[j].title, title) == 0) {
                            projects[idx_to].tasks = realloc(projects[idx_to].tasks, (projects[idx_to].task_count + 1) * sizeof(Task));
                            projects[idx_to].tasks[projects[idx_to].task_count] = projects[idx_from].tasks[j];
                            projects[idx_to].task_count++;
                            for (int k = j; k < projects[idx_from].task_count - 1; k++) {
                                projects[idx_from].tasks[k] = projects[idx_from].tasks[k + 1];
                            }
                            projects[idx_from].task_count--;
                            projects[idx_from].tasks = realloc(projects[idx_from].tasks, projects[idx_from].task_count * sizeof(Task));
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char p_name[256], title[256];
            if (sscanf(args, "%255s %255s", p_name, title) == 2) {
                for (int i = 0; i < p_count; i++) {
                    if (strcmp(projects[i].name, p_name) == 0) {
                        for (int j = 0; j < projects[i].task_count; j++) {
                            if (strcmp(projects[i].tasks[j].title, title) == 0) {
                                projects[i].tasks[j].done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < p_count; i++) {
                    if (strcmp(projects[i].name, name) == 0) {
                        free(projects[i].name);
                        for (int j = 0; j < projects[i].task_count; j++) {
                            free(projects[i].tasks[j].title);
                            free(projects[i].tasks[j].note);
                        }
                        free(projects[i].tasks);
                        for (int j = i; j < p_count - 1; j++) {
                            projects[j] = projects[j + 1];
                        }
                        p_count--;
                        projects = realloc(projects, p_count * sizeof(Project));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < p_count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("  Task: %s [%s] - %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
    }
    for (int i = 0; i < p_count; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].task_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
