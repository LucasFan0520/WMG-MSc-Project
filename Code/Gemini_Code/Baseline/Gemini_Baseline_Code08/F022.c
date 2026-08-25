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

Project *projects = NULL;
int project_count = 0;
int project_capacity = 0;

int find_project(const char *name) {
    for (int i = 0; i < project_count; i++) {
        if (strcmp(projects[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void create_project(const char *name) {
    if (find_project(name) != -1) return;
    if (project_count >= project_capacity) {
        project_capacity = project_capacity == 0 ? 4 : project_capacity * 2;
        projects = realloc(projects, project_capacity * sizeof(Project));
    }
    projects[project_count].name = strdup(name);
    projects[project_count].tasks = NULL;
    projects[project_count].task_count = 0;
    projects[project_count].task_capacity = 0;
    project_count++;
}

void add_task(const char *p_name, const char *title, const char *note) {
    int idx = find_project(p_name);
    if (idx == -1) return;
    Project *p = &projects[idx];
    if (p->task_count >= p->task_capacity) {
        p->task_capacity = p->task_capacity == 0 ? 4 : p->task_capacity * 2;
        p->tasks = realloc(p->tasks, p->task_capacity * sizeof(Task));
    }
    p->tasks[p->task_count].title = strdup(title);
    p->tasks[p->task_count].note = strdup(note);
    p->tasks[p->task_count].done = 0;
    p->task_count++;
}

void move_task(const char *from_p, const char *to_p, const char *title) {
    int idxF = find_project(from_p);
    int idxT = find_project(to_p);
    if (idxF == -1 || idxT == -1) return;
    Project *pF = &projects[idxF];
    Project *pT = &projects[idxT];
    int t_idx = -1;
    for (int i = 0; i < pF->task_count; i++) {
        if (strcmp(pF->tasks[i].title, title) == 0) {
            t_idx = i;
            break;
        }
    }
    if (t_idx == -1) return;
    if (pT->task_count >= pT->task_capacity) {
        pT->task_capacity = pT->task_capacity == 0 ? 4 : pT->task_capacity * 2;
        pT->tasks = realloc(pT->tasks, pT->task_capacity * sizeof(Task));
    }
    pT->tasks[pT->task_count] = pF->tasks[t_idx];
    pT->task_count++;
    for (int i = t_idx; i < pF->task_count - 1; i++) {
        pF->tasks[i] = pF->tasks[i + 1];
    }
    pF->task_count--;
}

void done_task(const char *p_name, const char *title) {
    int idx = find_project(p_name);
    if (idx == -1) return;
    Project *p = &projects[idx];
    for (int i = 0; i < p->task_count; i++) {
        if (strcmp(p->tasks[i].title, title) == 0) {
            p->tasks[i].done = 1;
            break;
        }
    }
}

void delete_project(const char *name) {
    int idx = find_project(name);
    if (idx == -1) return;
    free(projects[idx].name);
    for (int i = 0; i < projects[idx].task_count; i++) {
        free(projects[idx].tasks[i].title);
        free(projects[idx].tasks[i].note);
    }
    free(projects[idx].tasks);
    for (int i = idx; i < project_count - 1; i++) {
        projects[i] = projects[i + 1];
    }
    project_count--;
}

void report_board(void) {
    for (int i = 0; i < project_count; i++) {
        printf("PROJECT %s\n", projects[i].name);
        for (int j = 0; j < projects[i].task_count; j++) {
            printf("TASK %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
        }
    }
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = strtok(NULL, "");
            if (name) create_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char *p_name = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (p_name && title && note) add_task(p_name, title, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *from_p = strtok(NULL, " ");
            char *to_p = strtok(NULL, " ");
            char *title = strtok(NULL, "");
            if (from_p && to_p && title) move_task(from_p, to_p, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *p_name = strtok(NULL, " ");
            char *title = strtok(NULL, "");
            if (p_name && title) done_task(p_name, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_board();
        }
    }
    free(line);
    for (int i = 0; i < project_count; i++) {
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
