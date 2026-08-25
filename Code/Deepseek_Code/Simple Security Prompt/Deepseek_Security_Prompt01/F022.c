// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *note; int done; } Task;
typedef struct { char *name; Task *tasks; int tc, tcap; } Project;
Project *projects = NULL;
int pc = 0, pcap = 0;

int find_project(const char *name) {
    for (int i = 0; i < pc; i++)
        if (strcmp(projects[i].name, name) == 0) return i;
    return -1;
}

int find_task(Project *p, const char *title) {
    for (int i = 0; i < p->tc; i++)
        if (strcmp(p->tasks[i].title, title) == 0) return i;
    return -1;
}

void add_project(const char *name) {
    if (find_project(name) >= 0) return;
    if (pc == pcap) { pcap = pcap ? pcap * 2 : 4; projects = realloc(projects, sizeof(Project) * pcap); }
    projects[pc].name = strdup(name);
    projects[pc].tasks = NULL;
    projects[pc].tc = 0;
    projects[pc].tcap = 0;
    pc++;
}

void add_task(const char *proj, const char *title, const char *note) {
    int idx = find_project(proj);
    if (idx < 0) return;
    Project *p = &projects[idx];
    if (find_task(p, title) >= 0) return;
    if (p->tc == p->tcap) { p->tcap = p->tcap ? p->tcap * 2 : 4; p->tasks = realloc(p->tasks, sizeof(Task) * p->tcap); }
    p->tasks[p->tc].title = strdup(title);
    p->tasks[p->tc].note = strdup(note);
    p->tasks[p->tc].done = 0;
    p->tc++;
}

void move_task(const char *from, const char *to, const char *title) {
    int fi = find_project(from), ti = find_project(to);
    if (fi < 0 || ti < 0) return;
    Project *fp = &projects[fi];
    int tidx = find_task(fp, title);
    if (tidx < 0) return;
    add_task(to, fp->tasks[tidx].title, fp->tasks[tidx].note);
    if (find_task(&projects[ti], title) >= 0) {
        free(fp->tasks[tidx].title);
        free(fp->tasks[tidx].note);
        for (int i = tidx; i < fp->tc - 1; i++) fp->tasks[i] = fp->tasks[i + 1];
        fp->tc--;
    }
}

void done_task(const char *proj, const char *title) {
    int idx = find_project(proj);
    if (idx < 0) return;
    Project *p = &projects[idx];
    int tidx = find_task(p, title);
    if (tidx < 0) return;
    p->tasks[tidx].done = 1;
}

void delete_project(const char *name) {
    int idx = find_project(name);
    if (idx < 0) return;
    for (int i = 0; i < projects[idx].tc; i++) {
        free(projects[idx].tasks[i].title);
        free(projects[idx].tasks[i].note);
    }
    free(projects[idx].tasks);
    free(projects[idx].name);
    for (int i = idx; i < pc - 1; i++) projects[i] = projects[i + 1];
    pc--;
}

void report_projects(void) {
    for (int i = 0; i < pc; i++) {
        printf("%s\n", projects[i].name);
        for (int j = 0; j < projects[i].tc; j++) {
            printf("%s %s %s\n", projects[i].tasks[j].title,
                   projects[i].tasks[j].done ? "DONE" : "TODO",
                   projects[i].tasks[j].note);
        }
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) add_project(arg1);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", arg1, arg2, arg3) == 3)
                add_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", arg1, arg2, arg3) == 3)
                move_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                done_task(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_project(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) report_projects();
    }
    for (int i = 0; i < pc; i++) {
        for (int j = 0; j < projects[i].tc; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
        free(projects[i].name);
    }
    free(projects);
    return 0;
}