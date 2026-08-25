/* F022.c */
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
    size_t tcount, tcap;
} Project;

static Project *projects = NULL;
static size_t pcount = 0, pcap = 0;

static Project *find_project(const char *name) {
    for (size_t i = 0; i < pcount; i++) {
        if (strcmp(projects[i].name, name) == 0)
            return &projects[i];
    }
    return NULL;
}

static void add_project(const char *name) {
    if (pcount == pcap) {
        pcap = pcap ? pcap * 2 : 4;
        projects = realloc(projects, pcap * sizeof(Project));
        if (!projects) { perror("realloc"); exit(1); }
    }
    projects[pcount].name = strdup(name);
    projects[pcount].tasks = NULL;
    projects[pcount].tcount = projects[pcount].tcap = 0;
    pcount++;
}

static void add_task(Project *p, const char *title, const char *note) {
    if (p->tcount == p->tcap) {
        p->tcap = p->tcap ? p->tcap * 2 : 4;
        p->tasks = realloc(p->tasks, p->tcap * sizeof(Task));
        if (!p->tasks) { perror("realloc"); exit(1); }
    }
    p->tasks[p->tcount].title = strdup(title);
    p->tasks[p->tcount].note = strdup(note);
    p->tasks[p->tcount].done = 0;
    p->tcount++;
}

static Task *find_task_in_project(Project *p, const char *title) {
    for (size_t i = 0; i < p->tcount; i++) {
        if (strcmp(p->tasks[i].title, title) == 0)
            return &p->tasks[i];
    }
    return NULL;
}

static int move_task(const char *from, const char *to, const char *title) {
    Project *fp = find_project(from);
    Project *tp = find_project(to);
    if (!fp || !tp) return 0;
    Task *t = find_task_in_project(fp, title);
    if (!t) return 0;
    char *note = strdup(t->note);
    add_task(tp, title, note);
    free(note);
    // remove from fp
    for (size_t i = 0; i < fp->tcount; i++) {
        if (strcmp(fp->tasks[i].title, title) == 0) {
            free(fp->tasks[i].title);
            free(fp->tasks[i].note);
            for (size_t j = i; j < fp->tcount - 1; j++)
                fp->tasks[j] = fp->tasks[j + 1];
            fp->tcount--;
            return 1;
        }
    }
    return 0;
}

static void done_task(Project *p, const char *title) {
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

static void delete_project(const char *name) {
    for (size_t i = 0; i < pcount; i++) {
        if (strcmp(projects[i].name, name) == 0) {
            free(projects[i].name);
            for (size_t j = 0; j < projects[i].tcount; j++) {
                free(projects[i].tasks[j].title);
                free(projects[i].tasks[j].note);
            }
            free(projects[i].tasks);
            for (size_t j = i; j < pcount - 1; j++)
                projects[j] = projects[j + 1];
            pcount--;
            return;
        }
    }
}

static void report_projects(void) {
    for (size_t i = 0; i < pcount; i++) {
        printf("%s\n", projects[i].name);
        for (size_t j = 0; j < projects[i].tcount; j++) {
            printf("  %s %s %s\n", projects[i].tasks[j].title,
                   projects[i].tasks[j].note,
                   projects[i].tasks[j].done ? "done" : "todo");
        }
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            if (!find_project(name)) add_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char *pname = strtok(NULL, " ");
            if (!pname) continue;
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            Project *p = find_project(pname);
            if (p) add_task(p, title, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *from = strtok(NULL, " ");
            char *to = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (from && to && title) move_task(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *pname = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (pname && title) {
                Project *p = find_project(pname);
                if (p) done_task(p, title);
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    for (size_t i = 0; i < pcount; i++) {
        free(projects[i].name);
        for (size_t j = 0; j < projects[i].tcount; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}