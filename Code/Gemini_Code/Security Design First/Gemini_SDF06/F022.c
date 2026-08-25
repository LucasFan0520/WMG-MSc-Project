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
    size_t task_count;
    size_t task_capacity;
} Project;

int main(void) {
    Project *projects = NULL;
    size_t proj_count = 0;
    size_t proj_capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            if (proj_count >= proj_capacity) {
                size_t new_cap = proj_capacity == 0 ? 4 : proj_capacity * 2;
                Project *new_proj = realloc(projects, new_cap * sizeof(Project));
                if (!new_proj) continue;
                projects = new_proj;
                proj_capacity = new_cap;
            }
            char *pn = strdup(name);
            if (pn) {
                projects[proj_count].name = pn;
                projects[proj_count].tasks = NULL;
                projects[proj_count].task_count = 0;
                projects[proj_count].task_capacity = 0;
                proj_count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *proj_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;

            if (*proj_name == '\0' || *title == '\0') continue;

            for (size_t i = 0; i < proj_count; i++) {
                if (strcmp(projects[i].name, proj_name) == 0) {
                    Project *pr = &projects[i];
                    if (pr->task_count >= pr->task_capacity) {
                        size_t new_cap = pr->task_capacity == 0 ? 4 : pr->task_capacity * 2;
                        ProjectTask *new_tasks = realloc(pr->tasks, new_cap * sizeof(ProjectTask));
                        if (!new_tasks) break;
                        pr->tasks = new_tasks;
                        pr->task_capacity = new_cap;
                    }
                    char *ttl = strdup(title);
                    char *nt = strdup(note);
                    if (ttl && nt) {
                        pr->tasks[pr->task_count].title = ttl;
                        pr->tasks[pr->task_count].note = nt;
                        pr->tasks[pr->task_count].done = 0;
                        pr->task_count++;
                    } else {
                        free(ttl);
                        free(nt);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *from = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *to = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*from == '\0' || *to == '\0' || *title == '\0') continue;

            int f_idx = -1, t_idx = -1;
            for (size_t i = 0; i < proj_count; i++) {
                if (strcmp(projects[i].name, from) == 0) f_idx = (int)i;
                if (strcmp(projects[i].name, to) == 0) t_idx = (int)i;
            }
            if (f_idx != -1 && t_idx != -1 && f_idx != t_idx) {
                Project *pf = &projects[f_idx];
                Project *pt = &projects[t_idx];
                for (size_t j = 0; j < pf->task_count; j++) {
                    if (strcmp(pf->tasks[j].title, title) == 0) {
                        if (pt->task_count >= pt->task_capacity) {
                            size_t new_cap = pt->task_capacity == 0 ? 4 : pt->task_capacity * 2;
                            ProjectTask *new_t = realloc(pt->tasks, new_cap * sizeof(ProjectTask));
                            if (!new_t) break;
                            pt->tasks = new_t;
                            pt->task_capacity = new_cap;
                        }
                        pt->tasks[pt->task_count] = pf->tasks[j];
                        pt->task_count++;

                        for (size_t k = j; k < pf->task_count - 1; k++) {
                            pf->tasks[k] = pf->tasks[k + 1];
                        }
                        pf->task_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *proj_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*proj_name == '\0' || *title == '\0') continue;

            for (size_t i = 0; i < proj_count; i++) {
                if (strcmp(projects[i].name, proj_name) == 0) {
                    for (size_t j = 0; j < projects[i].task_count; j++) {
                        if (strcmp(projects[i].tasks[j].title, title) == 0) {
                            projects[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < proj_count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (size_t j = 0; j < projects[i].task_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (size_t j = i; j < proj_count - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    proj_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < proj_count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (size_t j = 0; j < projects[i].task_count; j++) {
                    printf("  Task: %s [%s] - %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
    }

    for (size_t i = 0; i < proj_count; i++) {
        free(projects[i].name);
        for (size_t j = 0; j < projects[i].task_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    free(line);
    return 0;
}
