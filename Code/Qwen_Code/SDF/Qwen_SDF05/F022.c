// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *title; char *note; int done; } Task;
typedef struct {
    char *name;
    Task *tasks;
    size_t tcnt, tcap;
} Project;

static Project *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Project *t = realloc(arr, nc * sizeof(Project));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int task_grow(Project *p) {
    if (p->tcnt < p->tcap) return 0;
    size_t nc = p->tcap ? p->tcap * 2 : 4;
    if (nc < p->tcap) return -1;
    Task *t = realloc(p->tasks, nc * sizeof(Task));
    if (!t) return -1;
    p->tasks = t; p->tcap = nc;
    return 0;
}

static int find_project(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void free_project(Project *p) {
    free(p->name);
    for (size_t i = 0; i < p->tcnt; i++) { free(p->tasks[i].title); free(p->tasks[i].note); }
    free(p->tasks);
    p->name = NULL; p->tasks = NULL; p->tcnt = 0; p->tcap = 0;
}

static void cmd_project(char *name) {
    if (find_project(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].tasks = NULL; arr[cnt].tcnt = 0; arr[cnt].tcap = 0;
    if (!arr[cnt].name) return;
    cnt++;
}

static void cmd_task(char *proj, char *title, char *note) {
    int idx = find_project(proj);
    if (idx < 0) return;
    Project *p = &arr[idx];
    if (task_grow(p) < 0) return;
    p->tasks[p->tcnt].title = safe_strdup(title);
    p->tasks[p->tcnt].note = safe_strdup(note);
    p->tasks[p->tcnt].done = 0;
    if (!p->tasks[p->tcnt].title || !p->tasks[p->tcnt].note) {
        free(p->tasks[p->tcnt].title); free(p->tasks[p->tcnt].note);
        p->tasks[p->tcnt].title = NULL; p->tasks[p->tcnt].note = NULL;
        return;
    }
    p->tcnt++;
}

static void cmd_movetask(char *from, char *to, char *title) {
    int ifr = find_project(from);
    int ito = find_project(to);
    if (ifr < 0 || ito < 0 || ifr == ito) return;
    Project *pf = &arr[ifr];
    Project *pt = &arr[ito];
    for (size_t i = 0; i < pf->tcnt; i++) {
        if (strcmp(pf->tasks[i].title, title) == 0) {
            if (task_grow(pt) < 0) return;
            pt->tasks[pt->tcnt] = pf->tasks[i];
            pt->tcnt++;
            for (size_t j = i; j + 1 < pf->tcnt; j++) pf->tasks[j] = pf->tasks[j + 1];
            pf->tcnt--;
            return;
        }
    }
}

static void cmd_donetask(char *proj, char *title) {
    int idx = find_project(proj);
    if (idx < 0) return;
    Project *p = &arr[idx];
    for (size_t i = 0; i < p->tcnt; i++) {
        if (strcmp(p->tasks[i].title, title) == 0) {
            p->tasks[i].done = 1;
            return;
        }
    }
}

static void cmd_deleteproject(char *name) {
    int idx = find_project(name);
    if (idx < 0) return;
    free_project(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++) {
        printf("%s\n", arr[i].name);
        for (size_t j = 0; j < arr[i].tcnt; j++)
            printf("  %s %s %s\n", arr[i].tasks[j].title, arr[i].tasks[j].note, arr[i].tasks[j].done ? "DONE" : "TODO");
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_project(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[256] = {0}, title[256] = {0}; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%255s%n %255s%n", proj, &n2, title, &n3) < 2) continue;
            char *note = rest + n3;
            while (*note == ' ') note++;
            cmd_task(proj, title, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256] = {0}, to[256] = {0}, title[256] = {0};
            if (sscanf(rest, "%255s %255s %255s", from, to, title) < 3) continue;
            cmd_movetask(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[256] = {0}, title[256] = {0};
            if (sscanf(rest, "%255s %255s", proj, title) < 2) continue;
            cmd_donetask(proj, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_deleteproject(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}