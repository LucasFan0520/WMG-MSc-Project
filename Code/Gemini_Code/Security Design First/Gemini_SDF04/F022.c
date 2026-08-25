// F022.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int done;
} ProjectTask;

typedef struct {
    char *name;
    ProjectTask *tasks;
    int task_count;
    int task_cap;
} Project;

Project *projects = NULL;
int proj_count = 0;
int proj_cap = 0;

int find_project(const char *name) {
    for (int i = 0; i < proj_count; i++) {
        if (strcmp(projects[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "PROJECT") == 0) {
                if (find_project(arg1) == -1) {
                    if (proj_count >= proj_cap) {
                        proj_cap = proj_cap == 0 ? 4 : proj_cap * 2;
                        Project *tmp = realloc(projects, proj_cap * sizeof(Project));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        projects = tmp;
                    }
                    projects[proj_count].name = strdup(arg1);
                    projects[proj_count].tasks = NULL;
                    projects[proj_count].task_count = 0;
                    projects[proj_count].task_cap = 0;
                    if (projects[proj_count].name) {
                        proj_count++;
                    }
                }
            } else if (strcmp(cmd, "TASK") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *title = space2 + 1;
                    char *space3 = strchr(title, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *note = space3 + 1;
                        int idx = find_project(arg1);
                        if (idx != -1) {
                            Project *p = &projects[idx];
                            if (p->task_count >= p->task_cap) {
                                p->task_cap = p->task_cap == 0 ? 4 : p->task_cap * 2;
                                ProjectTask *tmp = realloc(p->tasks, p->task_cap * sizeof(ProjectTask));
                                if (!tmp) {
                                    free(line);
                                    return 1;
                                }
                                p->tasks = tmp;
                            }
                            p->tasks[p->task_count].title = strdup(title);
                            p->tasks[p->task_count].note = strdup(note);
                            p->tasks[p->task_count].done = 0;
                            if (p->tasks[p->task_count].title && p->tasks[p->task_count].note) {
                                p->task_count++;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "MOVETASK") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *to_proj = space2 + 1;
                    char *space3 = strchr(to_proj, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *title = space3 + 1;
                        int idx_from = find_project(arg1);
                        int idx_to = find_project(to_proj);
                        if (idx_from != -1 && idx_to != -1 && idx_from != idx_to) {
                            Project *pf = &projects[idx_from];
                            Project *pt = &projects[idx_to];
                            for (int i = 0; i < pf->task_count; i++) {
                                if (strcmp(pf->tasks[i].title, title) == 0) {
                                    if (pt->task_count >= pt->task_cap) {
                                        pt->task_cap = pt->task_cap == 0 ? 4 : pt->task_cap * 2;
                                        ProjectTask *tmp = realloc(pt->tasks, pt->task_cap * sizeof(ProjectTask));
                                        if (!tmp) {
                                            free(line);
                                            return 1;
                                        }
                                        pt->tasks = tmp;
                                    }
                                    pt->tasks[pt->task_count] = pf->tasks[i];
                                    pt->task_count++;
                                    for (int j = i; j < pf->task_count - 1; j++) {
                                        pf->tasks[j] = pf->tasks[j + 1];
                                    }
                                    pf->task_count--;
                                    break;
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DONETASK") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *title = space2 + 1;
                    int idx = find_project(arg1);
                    if (idx != -1) {
                        Project *p = &projects[idx];
                        for (int i = 0; i < p->task_count; i++) {
                            if (strcmp(p->tasks[i].title, title) == 0) {
                                p->tasks[i].done = 1;
                                break;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
                int idx = find_project(arg1);
                if (idx != -1) {
                    free(projects[idx].name);
                    for (int i = 0; i < projects[idx].task_count; i++) {
                        free(projects[idx].tasks[i].title);
                        free(projects[idx].tasks[i].note);
                    }
                    free(projects[idx].tasks);
                    for (int i = idx; i < proj_count - 1; i++) {
                        projects[i] = projects[i + 1];
                    }
                    proj_count--;
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < proj_count; i++) {
                    printf("Project %s:\n", projects[i].name);
                    for (int j = 0; j < projects[i].task_count; j++) {
                        printf("  Task: %s [%s] %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                    }
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < proj_count; i++) {
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