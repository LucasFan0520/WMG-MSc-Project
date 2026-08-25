// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *title; char *note; int done; } Task;
typedef struct { char *name; Task *tasks; size_t tc; size_t tcap; } Proj;
Proj *projs = NULL;
size_t pc = 0, pcap = 0;

Proj* find_proj(const char *name) {
    for (size_t i = 0; i < pc; i++) if (strcmp(projs[i].name, name) == 0) return &projs[i];
    return NULL;
}

void add_proj(const char *name) {
    if (find_proj(name)) return;
    if (pc == pcap) {
        size_t nc = pcap ? pcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Proj)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Proj *tmp = realloc(projs, nc * sizeof(Proj));
        if (!tmp) { perror("realloc"); exit(1); }
        projs = tmp;
        pcap = nc;
    }
    projs[pc].name = strdup(name);
    projs[pc].tasks = NULL;
    projs[pc].tc = 0;
    projs[pc].tcap = 0;
    if (!projs[pc].name) { perror("strdup"); exit(1); }
    pc++;
}

void add_task(Proj *p, const char *title, const char *note) {
    if (p->tc == p->tcap) {
        size_t nc = p->tcap ? p->tcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Task)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Task *tmp = realloc(p->tasks, nc * sizeof(Task));
        if (!tmp) { perror("realloc"); exit(1); }
        p->tasks = tmp;
        p->tcap = nc;
    }
    p->tasks[p->tc].title = strdup(title);
    p->tasks[p->tc].note = strdup(note);
    p->tasks[p->tc].done = 0;
    if (!p->tasks[p->tc].title || !p->tasks[p->tc].note) { perror("strdup"); exit(1); }
    p->tc++;
}

void del_proj(const char *name) {
    for (size_t i = 0; i < pc; i++) {
        if (strcmp(projs[i].name, name) == 0) {
            free(projs[i].name);
            for (size_t j = 0; j < projs[i].tc; j++) { free(projs[i].tasks[j].title); free(projs[i].tasks[j].note); }
            free(projs[i].tasks);
            projs[i] = projs[--pc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) add_proj(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[256], title[256];
            if (sscanf(line, "%*s %255s %255s", proj, title) == 2) {
                Proj *p = find_proj(proj);
                if (p) {
                    char *note = strchr(line, ' ');
                    if (note) { note = strchr(note + 1, ' '); if (note) note = strchr(note + 1, ' '); if (note) note++; }
                    if (!note) note = "";
                    add_task(p, title, note);
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char fp[256], tp[256], title[256];
            if (sscanf(line, "%*s %255s %255s %255s", fp, tp, title) == 3) {
                Proj *src = find_proj(fp), *dst = find_proj(tp);
                if (src && dst) {
                    for (size_t i = 0; i < src->tc; i++) {
                        if (strcmp(src->tasks[i].title, title) == 0) {
                            add_task(dst, src->tasks[i].title, src->tasks[i].note);
                            dst->tasks[dst->tc-1].done = src->tasks[i].done;
                            free(src->tasks[i].title); free(src->tasks[i].note);
                            src->tasks[i] = src->tasks[--src->tc];
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[256], title[256];
            if (sscanf(line, "%*s %255s %255s", proj, title) == 2) {
                Proj *p = find_proj(proj);
                if (p) {
                    for (size_t i = 0; i < p->tc; i++) {
                        if (strcmp(p->tasks[i].title, title) == 0) { p->tasks[i].done = 1; break; }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_proj(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < pc; i++) {
                printf("%s\n", projs[i].name);
                for (size_t j = 0; j < projs[i].tc; j++) {
                    printf(" %s %s %s\n", projs[i].tasks[j].done ? "DONE" : "TODO", projs[i].tasks[j].title, projs[i].tasks[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < pc; i++) {
        free(projs[i].name);
        for (size_t j = 0; j < projs[i].tc; j++) { free(projs[i].tasks[j].title); free(projs[i].tasks[j].note); }
        free(projs[i].tasks);
    }
    free(projs);
    return 0;
}