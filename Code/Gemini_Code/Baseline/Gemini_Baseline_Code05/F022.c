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
    int task_capacity;
} Project;

Project *projects = NULL;
int p_count = 0;
int p_capacity = 0;

int find_project(const char *name) {
    for (int i = 0; i < p_count; i++) {
        if (strcmp(projects[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void add_project(const char *name) {
    if (find_project(name) != -1) {
        return;
    }
    if (p_count >= p_capacity) {
        p_capacity = p_capacity == 0 ? 4 : p_capacity * 2;
        projects = realloc(projects, p_capacity * sizeof(Project));
    }
    projects[p_count].name = strdup(name);
    projects[p_count].tasks = NULL;
    projects[p_count].task_count = 0;
    projects[p_count].task_capacity = 0;
    p_count++;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            add_project(line + 8);
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p1 = line + 5;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *note = sp2 + 1;
                    int p_idx = find_project(p1);
                    if (p_idx != -1) {
                        Project *p = &projects[p_idx];
                        if (r_capacity == 0) {} 
                        if (p->task_count >= p->task_capacity) {
                            p->task_capacity = p->task_capacity == 0 ? 4 : p->task_capacity * 2;
                            p->tasks = realloc(p->tasks, p->task_capacity * sizeof(Task));
                        }
                        p->tasks[p->task_count].title = strdup(p2);
                        p->tasks[p->task_count].note = strdup(note);
                        p->tasks[p->task_count].done = 0;
                        p->task_count++;
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p1 = line + 9;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *title = sp2 + 1;
                    int idx_from = find_project(p1);
                    int idx_to = find_project(p2);
                    if (idx_from != -1 && idx_to != -1) {
                        Project *pf = &projects[idx_from];
                        Project *pt = &projects[idx_to];
                        int t_idx = -1;
                        for (int i = 0; i < pf->task_count; i++) {
                            if (strcmp(pf->tasks[i].title, title) == 0) {
                                t_idx = i;
                                break;
                            }
                        }
                        if (t_idx != -1) {
                            if (pt->task_count >= pt->task_capacity) {
                                pt->task_capacity = pt->task_capacity == 0 ? 4 : pt->task_capacity * 2;
                                pt->tasks = realloc(pt->tasks, pt->task_capacity * sizeof(Task));
                            }
                            pt->tasks[pt->task_count] = pf->tasks[t_idx];
                            pt->task_count++;
                            for (int i = t_idx; i < pf->task_count - 1; i++) {
                                pf->tasks[i] = pf->tasks[i + 1];
                            }
                            pf->task_count--;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p1 = line + 9;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *title = sp + 1;
                int p_idx = find_project(p1);
                if (p_idx != -1) {
                    Project *p = &projects[p_idx];
                    for (int i = 0; i < p->task_count; i++) {
                        if (strcmp(p->tasks[i].title, title) == 0) {
                            p->tasks[i].done = 1;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
            int p_idx = find_project(name);
            if (p_idx != -1) {
                free(projects[p_idx].name);
                for (int i = 0; i < projects[p_idx].task_count; i++) {
                    free(projects[p_idx].tasks[i].title);
                    free(projects[p_idx].tasks[i].note);
                }
                free(projects[p_idx].tasks);
                for (int i = p_idx; i < p_count - 1; i++) {
                    projects[i] = projects[i + 1];
                }
                p_count--;
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < p_count; i++) {
                printf("PROJECT %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("TASK %s %s [%s]\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
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
