/* F022.c */
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

int main(void) {
    Project *projects = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            char *n_name = strdup(name);
            if (n_name) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Project *tmp = realloc(projects, new_cap * sizeof(Project));
                    if (tmp) {
                        projects = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name);
                        continue;
                    }
                }
                projects[count].name = n_name;
                projects[count].tasks = NULL;
                projects[count].task_count = 0;
                projects[count].task_cap = 0;
                count++;
            }
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *args = line + 5;
            char *space1 = strchr(args, ' ');
            if (space1) {
                *space1 = '\0';
                char *pname = args;
                char *rest = space1 + 1;
                char *space2 = strchr(rest, ' ');
                char *t_title = NULL;
                char *t_note = NULL;
                if (space2) {
                    *space2 = '\0';
                    t_title = rest;
                    t_note = space2 + 1;
                } else {
                    t_title = rest;
                    t_note = "";
                }
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, pname) == 0) {
                        Project *p = &projects[i];
                        char *n_title = strdup(t_title);
                        char *n_note = strdup(t_note);
                        if (n_title && n_note) {
                            if (p->task_count >= p->task_cap) {
                                int new_tcap = p->task_cap == 0 ? 4 : p->task_cap * 2;
                                ProjectTask *tmp = realloc(p->tasks, new_tcap * sizeof(ProjectTask));
                                if (tmp) {
                                    p->tasks = tmp;
                                    p->task_cap = new_tcap;
                                } else {
                                    free(n_title);
                                    free(n_note);
                                    break;
                                }
                            }
                            p->tasks[p->task_count].title = n_title;
                            p->tasks[p->task_count].note = n_note;
                            p->tasks[p->task_count].done = 0;
                            p->task_count++;
                        } else {
                            free(n_title);
                            free(n_note);
                        }
                        break;
                    }
                }
                *space1 = ' ';
                if (space2) *space2 = ' ';
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *args = line + 9;
            char *space1 = strchr(args, ' ');
            if (space1) {
                *space1 = '\0';
                char *fromP = args;
                char *rest = space1 + 1;
                char *space2 = strchr(rest, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *toP = rest;
                    char *t_title = space2 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, fromP) == 0) f_idx = i;
                        if (strcmp(projects[i].name, toP) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        Project *fp = &projects[f_idx];
                        Project *tp = &projects[t_idx];
                        for (int j = 0; j < fp->task_count; j++) {
                            if (strcmp(fp->tasks[j].title, t_title) == 0) {
                                if (tp->task_count >= tp->task_cap) {
                                    int new_tcap = tp->task_cap == 0 ? 4 : tp->task_cap * 2;
                                    ProjectTask *tmp = realloc(tp->tasks, new_tcap * sizeof(ProjectTask));
                                    if (tmp) {
                                        tp->tasks = tmp;
                                        tp->task_cap = new_tcap;
                                    } else {
                                        break;
                                    }
                                }
                                tp->tasks[tp->task_count] = fp->tasks[j];
                                tp->task_count++;
                                for (int k = j; k < fp->task_count - 1; k++) {
                                    fp->tasks[k] = fp->tasks[k + 1];
                                }
                                fp->task_count--;
                                break;
                            }
                        }
                    }
                    *space2 = ' ';
                }
                *space1 = ' ';
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *args = line + 9;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *pname = args;
                char *t_title = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, pname) == 0) {
                        for (int j = 0; j < projects[i].task_count; j++) {
                            if (strcmp(projects[i].tasks[j].title, t_title) == 0) {
                                projects[i].tasks[j].done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *pname = line + 14;
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, pname) == 0) {
                    free(projects[i].name);
                    for (int j = 0; j < projects[i].task_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (int j = i; j < count - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("PROJECT %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("TASK %s %s [%s]\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
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
