// F022.c
#define _GNU_SOURCE
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

int main(void) {
    Project *projects = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            while (*name == ' ') name++;
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Project *tmp = realloc(projects, new_cap * sizeof(Project));
                if (!tmp) return 1;
                projects = tmp;
                capacity = new_cap;
            }
            projects[count].name = strdup(name);
            projects[count].tasks = NULL;
            projects[count].task_count = 0;
            projects[count].task_capacity = 0;
            count++;
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *pname = p;
            char *space1 = strchr(pname, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *title = space1 + 1;
            while (*title == ' ') title++;
            char *space2 = strchr(title, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            int p_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, pname) == 0) {
                    p_idx = i;
                    break;
                }
            }
            if (p_idx != -1) {
                Project *pr = &projects[p_idx];
                if (pr->task_count >= pr->task_capacity) {
                    int new_tcap = pr->task_capacity == 0 ? 4 : pr->task_capacity * 2;
                    Task *tmp = realloc(pr->tasks, new_tcap * sizeof(Task));
                    if (!tmp) return 1;
                    pr->tasks = tmp;
                    pr->task_capacity = new_tcap;
                }
                pr->tasks[pr->task_count].title = strdup(title);
                pr->tasks[pr->task_count].note = strdup(note);
                pr->tasks[pr->task_count].done = 0;
                pr->task_count++;
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *fromP = p;
            char *space1 = strchr(fromP, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *toP = space1 + 1;
            while (*toP == ' ') toP++;
            char *space2 = strchr(toP, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *title = space2 + 1;
            while (*title == ' ') title++;
            
            int f_idx = -1, t_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, fromP) == 0) f_idx = i;
                if (strcmp(projects[i].name, toP) == 0) t_idx = i;
            }
            if (f_idx != -1 && t_idx != -1) {
                Project *fp = &projects[f_idx];
                Project *tp = &projects[t_idx];
                int tk_idx = -1;
                for (int j = 0; j < fp->task_count; j++) {
                    if (strcmp(fp->tasks[j].title, title) == 0) {
                        tk_idx = j;
                        break;
                    }
                }
                if (tk_idx != -1) {
                    if (tp->task_count >= tp->task_capacity) {
                        int new_tcap = tp->task_capacity == 0 ? 4 : tp->task_capacity * 2;
                        Task *tmp = realloc(tp->tasks, new_tcap * sizeof(Task));
                        if (!tmp) return 1;
                        tp->tasks = tmp;
                        tp->task_capacity = new_tcap;
                    }
                    tp->tasks[tp->task_count] = fp->tasks[tk_idx];
                    tp->task_count++;
                    for (int j = tk_idx; j < fp->task_count - 1; j++) {
                        fp->tasks[j] = fp->tasks[j + 1];
                    }
                    fp->task_count--;
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *pname = p;
            char *space = strchr(pname, ' ');
            if (!space) continue;
            *space = '\0';
            char *title = space + 1;
            while (*title == ' ') title++;
            int p_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, pname) == 0) {
                    p_idx = i;
                    break;
                }
            }
            if (p_idx != -1) {
                Project *pr = &projects[p_idx];
                for (int j = 0; j < pr->task_count; j++) {
                    if (strcmp(pr->tasks[j].title, title) == 0) {
                        pr->tasks[j].done = 1;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
            while (*name == ' ') name++;
            int p_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    p_idx = i;
                    break;
                }
            }
            if (p_idx != -1) {
                free(projects[p_idx].name);
                for (int j = 0; j < projects[p_idx].task_count; j++) {
                    free(projects[p_idx].tasks[j].title);
                    free(projects[p_idx].tasks[j].note);
                }
                free(projects[p_idx].tasks);
                for (int i = p_idx; i < count - 1; i++) {
                    projects[i] = projects[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("Task: %s [%s] %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
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
