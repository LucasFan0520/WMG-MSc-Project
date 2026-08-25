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
    int num_tasks;
} Project;

Project *projects = NULL;
int num_projects = 0;

int find_project(const char *name) {
    for (int i = 0; i < num_projects; i++) {
        if (strcmp(projects[i].name, name) == 0) return i;
    }
    return -1;
}

int find_task(Project *p, const char *title) {
    for (int i = 0; i < p->num_tasks; i++) {
        if (strcmp(p->tasks[i].title, title) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            projects = realloc(projects, (num_projects + 1) * sizeof(Project));
            projects[num_projects].name = strdup(name);
            projects[num_projects].tasks = NULL;
            projects[num_projects].num_tasks = 0;
            num_projects++;
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[256], title[256];
            sscanf(line, "%*s %s %s", proj, title);
            char *note = strstr(line, title);
            if (note) {
                note += strlen(title);
                if (*note == ' ') note++;
            } else {
                note = "";
            }
            int pidx = find_project(proj);
            if (pidx >= 0) {
                projects[pidx].tasks = realloc(projects[pidx].tasks, (projects[pidx].num_tasks + 1) * sizeof(Task));
                projects[pidx].tasks[projects[pidx].num_tasks].title = strdup(title);
                projects[pidx].tasks[projects[pidx].num_tasks].note = strdup(note);
                projects[pidx].tasks[projects[pidx].num_tasks].done = 0;
                projects[pidx].num_tasks++;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            sscanf(line, "%*s %s %s %s", from, to, title);
            int pidx_from = find_project(from);
            int pidx_to = find_project(to);
            if (pidx_from >= 0 && pidx_to >= 0) {
                int tidx = find_task(&projects[pidx_from], title);
                if (tidx >= 0) {
                    projects[pidx_to].tasks = realloc(projects[pidx_to].tasks, (projects[pidx_to].num_tasks + 1) * sizeof(Task));
                    projects[pidx_to].tasks[projects[pidx_to].num_tasks] = projects[pidx_from].tasks[tidx];
                    projects[pidx_to].num_tasks++;
                    
                    free(projects[pidx_from].tasks[tidx].title);
                    free(projects[pidx_from].tasks[tidx].note);
                    for (int i = tidx; i < projects[pidx_from].num_tasks - 1; i++) {
                        projects[pidx_from].tasks[i] = projects[pidx_from].tasks[i + 1];
                    }
                    projects[pidx_from].num_tasks--;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[256], title[256];
            sscanf(line, "%*s %s %s", proj, title);
            int pidx = find_project(proj);
            if (pidx >= 0) {
                int tidx = find_task(&projects[pidx], title);
                if (tidx >= 0) {
                    projects[pidx].tasks[tidx].done = 1;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int pidx = find_project(name);
            if (pidx >= 0) {
                for (int i = 0; i < projects[pidx].num_tasks; i++) {
                    free(projects[pidx].tasks[i].title);
                    free(projects[pidx].tasks[i].note);
                }
                free(projects[pidx].tasks);
                free(projects[pidx].name);
                for (int i = pidx; i < num_projects - 1; i++) {
                    projects[i] = projects[i + 1];
                }
                num_projects--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < num_projects; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].num_tasks; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
    }
    
    for (int i = 0; i < num_projects; i++) {
        for (int j = 0; j < projects[i].num_tasks; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
        free(projects[i].name);
    }
    free(projects);
    
    return 0;
}