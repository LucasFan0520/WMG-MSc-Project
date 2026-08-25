/* F022.c */
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
    int tcount;
    int tcap;
} Project;

static Project *projects = NULL;
static int pcount = 0;
static int pcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_project(const char *name) {
    for (int i = 0; i < pcount; i++)
        if (strcmp(projects[i].name, name) == 0) return i;
    return -1;
}

static int find_task(Project *p, const char *title) {
    for (int i = 0; i < p->tcount; i++)
        if (strcmp(p->tasks[i].title, title) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            if (find_project(name) >= 0) continue;
            if (pcount >= pcap) {
                pcap = pcap == 0 ? 8 : pcap * 2;
                projects = realloc(projects, sizeof(Project) * pcap);
            }
            projects[pcount].name = dup_str(name);
            projects[pcount].tasks = NULL;
            projects[pcount].tcount = 0;
            projects[pcount].tcap = 0;
            pcount++;
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[256] = {0}, title[256] = {0};
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, "%255s%n %255s%n", proj, &off2, title, &off3) != 2) continue;
            const char *note = line + offset + off2 + off3;
            while (*note == ' ') note++;
            int pi = find_project(proj);
            if (pi < 0) continue;
            Project *p = &projects[pi];
            if (p->tcount >= p->tcap) {
                p->tcap = p->tcap == 0 ? 4 : p->tcap * 2;
                p->tasks = realloc(p->tasks, sizeof(Task) * p->tcap);
            }
            p->tasks[p->tcount].title = dup_str(title);
            p->tasks[p->tcount].note = dup_str(note);
            p->tasks[p->tcount].done = 0;
            p->tcount++;
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256] = {0}, to[256] = {0}, title[256] = {0};
            if (sscanf(line + offset, "%255s %255s %255s", from, to, title) != 3) continue;
            int fi = find_project(from);
            int ti = find_project(to);
            if (fi < 0 || ti < 0) continue;
            Project *fp = &projects[fi];
            Project *tp = &projects[ti];
            int tidx = find_task(fp, title);
            if (tidx < 0) continue;
            Task t = fp->tasks[tidx];
            for (int i = tidx; i < fp->tcount - 1; i++) fp->tasks[i] = fp->tasks[i + 1];
            fp->tcount--;
            if (tp->tcount >= tp->tcap) {
                tp->tcap = tp->tcap == 0 ? 4 : tp->tcap * 2;
                tp->tasks = realloc(tp->tasks, sizeof(Task) * tp->tcap);
            }
            tp->tasks[tp->tcount++] = t;
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[256] = {0}, title[256] = {0};
            if (sscanf(line + offset, "%255s %255s", proj, title) != 2) continue;
            int pi = find_project(proj);
            if (pi < 0) continue;
            int tidx = find_task(&projects[pi], title);
            if (tidx >= 0) projects[pi].tasks[tidx].done = 1;
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int pi = find_project(name);
            if (pi < 0) continue;
            free(projects[pi].name);
            for (int i = 0; i < projects[pi].tcount; i++) {
                free(projects[pi].tasks[i].title);
                free(projects[pi].tasks[i].note);
            }
            free(projects[pi].tasks);
            for (int i = pi; i < pcount - 1; i++) projects[i] = projects[i + 1];
            pcount--;
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < pcount; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].tcount; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
    }
    for (int i = 0; i < pcount; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].tcount; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}