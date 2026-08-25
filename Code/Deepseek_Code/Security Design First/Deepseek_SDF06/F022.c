// F022.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    int done;
} Task;

typedef struct Project {
    char *name;
    Task *tasks;
    size_t task_count;
    size_t task_cap;
} Project;

static Project *projects = NULL;
static size_t project_count = 0;
static size_t project_cap = 0;

static ssize_t find_project(const char *name) {
    for (size_t i = 0; i < project_count; i++)
        if (strcmp(projects[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static ssize_t find_task_in_project(Project *p, const char *title) {
    for (size_t i = 0; i < p->task_count; i++)
        if (strcmp(p->tasks[i].title, title) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_task_to_project(Project *p, const char *title, const char *note) {
    if (find_task_in_project(p, title) != -1) return;
    if (p->task_count == p->task_cap) {
        p->task_cap = p->task_cap ? p->task_cap * 2 : 4;
        Task *new = realloc(p->tasks, p->task_cap * sizeof(Task));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        p->tasks = new;
    }
    p->tasks[p->task_count].title = strdup(title);
    p->tasks[p->task_count].note = strdup(note);
    p->tasks[p->task_count].done = 0;
    if (!p->tasks[p->task_count].title || !p->tasks[p->task_count].note)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    p->task_count++;
}

static void remove_task_from_project(Project *p, size_t idx) {
    free(p->tasks[idx].title);
    free(p->tasks[idx].note);
    for (size_t i = idx; i < p->task_count - 1; i++)
        p->tasks[i] = p->tasks[i + 1];
    p->task_count--;
}

static void add_project(const char *name) {
    if (find_project(name) != -1) return;
    if (project_count == project_cap) {
        project_cap = project_cap ? project_cap * 2 : 4;
        Project *new = realloc(projects, project_cap * sizeof(Project));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        projects = new;
    }
    projects[project_count].name = strdup(name);
    if (!projects[project_count].name) { fprintf(stderr, "Memory error\n"); exit(1); }
    projects[project_count].tasks = NULL;
    projects[project_count].task_count = 0;
    projects[project_count].task_cap = 0;
    project_count++;
}

static void move_task(const char *from, const char *to, const char *title) {
    ssize_t fi = find_project(from);
    ssize_t ti = find_project(to);
    if (fi == -1 || ti == -1) return;
    ssize_t tidx = find_task_in_project(&projects[fi], title);
    if (tidx == -1) return;
    // copy to destination
    add_task_to_project(&projects[ti], projects[fi].tasks[tidx].title,
                        projects[fi].tasks[tidx].note);
    // mark done? Not required, but we keep note; the new task is not done
    // remove from source
    remove_task_from_project(&projects[fi], tidx);
}

static void done_task(const char *proj, const char *title) {
    ssize_t pi = find_project(proj);
    if (pi == -1) return;
    ssize_t tidx = find_task_in_project(&projects[pi], title);
    if (tidx == -1) return;
    projects[pi].tasks[tidx].done = 1;
}

static void delete_project(const char *name) {
    ssize_t idx = find_project(name);
    if (idx == -1) return;
    free(projects[idx].name);
    for (size_t i = 0; i < projects[idx].task_count; i++) {
        free(projects[idx].tasks[i].title);
        free(projects[idx].tasks[i].note);
    }
    free(projects[idx].tasks);
    for (size_t i = idx; i < project_count - 1; i++)
        projects[i] = projects[i + 1];
    project_count--;
}

static void report(void) {
    for (size_t i = 0; i < project_count; i++) {
        printf("%s\n", projects[i].name);
        for (size_t j = 0; j < projects[i].task_count; j++)
            printf("%s %s %s\n", projects[i].tasks[j].title,
                   projects[i].tasks[j].done ? "done" : "todo",
                   projects[i].tasks[j].note);
    }
}

static void free_all(void) {
    for (size_t i = 0; i < project_count; i++) {
        free(projects[i].name);
        for (size_t j = 0; j < projects[i].task_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], arg1[256], arg2[256], note[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "PROJECT") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                add_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", name, arg1, note) == 3) {
                ssize_t idx = find_project(name);
                if (idx != -1)
                    add_task_to_project(&projects[idx], arg1, note);
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", name, arg1, arg2) == 3)
                move_task(name, arg1, arg2);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, arg1) == 2)
                done_task(name, arg1);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }

    free(line);
    free_all();
    return 0;
}