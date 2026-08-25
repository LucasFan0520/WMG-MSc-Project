// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Task {
    char *title;
    char *note;
    int done;
    struct Task *next;
} Task;

typedef struct Proj {
    char *name;
    Task *tasks;
    struct Proj *next;
} Proj;

Proj *projs = NULL;

Proj *find_proj(const char *name) {
    for (Proj *p = projs; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
    }
    return NULL;
}

void add_proj(const char *name) {
    if (find_proj(name)) return;
    Proj *p = malloc(sizeof(Proj));
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projs;
    projs = p;
}

void add_task(const char *pname, const char *title, const char *note) {
    Proj *p = find_proj(pname);
    if (!p) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Proj *pf = find_proj(from);
    Proj *pt = find_proj(to);
    if (!pf || !pt) return;
    Task *prev = NULL, *curr = pf->tasks;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else pf->tasks = curr->next;
            curr->next = pt->tasks;
            pt->tasks = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void done_task(const char *pname, const char *title) {
    Proj *p = find_proj(pname);
    if (!p) return;
    for (Task *t = p->tasks; t; t = t->next) {
        if (strcmp(t->title, title) == 0) {
            t->done = 1;
            return;
        }
    }
}

void delete_proj(const char *name) {
    Proj *prev = NULL, *curr = projs;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else projs = curr->next;
            Task *t = curr->tasks;
            while (t) {
                Task *next = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report() {
    for (Proj *p = projs; p; p = p->next) {
        printf("%s:\n", p->name);
        for (Task *t = p->tasks; t; t = t->next) {
            printf("  %s [%s] %s\n", t->title, t->done ? "DONE" : "TODO", t->note);
        }
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0}, arg3[256] = {0};
        sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        
        if (strcmp(cmd, "PROJECT") == 0) add_proj(arg1);
        else if (strcmp(cmd, "TASK") == 0) {
            char *note = get_rest(line, 3);
            if (note) add_task(arg1, arg2, note);
        }
        else if (strcmp(cmd, "MOVETASK") == 0) move_task(arg1, arg2, arg3);
        else if (strcmp(cmd, "DONETASK") == 0) done_task(arg1, arg2);
        else if (strcmp(cmd, "DELETEPROJECT") == 0) delete_proj(arg1);
        else if (strcmp(cmd, "REPORT") == 0) report();
        
        free(line);
    }
    return 0;
}