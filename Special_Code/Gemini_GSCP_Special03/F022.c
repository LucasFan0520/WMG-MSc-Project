// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char *note;
    int done;
} ProjectTask;

typedef struct {
    char *name;
    ProjectTask *tasks;
    size_t tcount;
    size_t tcap;
} Project;

int main(void) {
    Project *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Project *nitems = realloc(items, cap * sizeof(Project));
                if (nitems) items = nitems;
            }
            items[count].name = mystrdup(name);
            items[count].tasks = NULL;
            items[count].tcount = 0;
            items[count].tcap = 0;
            count++;
        } else if (strcmp(cmd, "TASK") == 0) {
            char *project = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, project) == 0) {
                    Project *pr = &items[i];
                    if (pr->tcount >= pr->tcap) {
                        pr->tcap = pr->tcap == 0 ? 4 : pr->tcap * 2;
                        ProjectTask *nt = realloc(pr->tasks, pr->tcap * sizeof(ProjectTask));
                        if (nt) pr->tasks = nt;
                    }
                    pr->tasks[pr->tcount].title = mystrdup(title);
                    pr->tasks[pr->tcount].note = mystrdup(note);
                    pr->tasks[pr->tcount].done = 0;
                    pr->tcount++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromProj = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *toProj = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            int fidx = -1, tidx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, fromProj) == 0) fidx = (int)i;
                if (strcmp(items[i].name, toProj) == 0) tidx = (int)i;
            }
            if (fidx != -1 && tidx != -1) {
                Project *fp = &items[fidx];
                Project *tp = &items[tidx];
                for (size_t j = 0; j < fp->tcount; j++) {
                    if (strcmp(fp->tasks[j].title, title) == 0) {
                        if (tp->tcount >= tp->tcap) {
                            tp->tcap = tp->tcap == 0 ? 4 : tp->tcap * 2;
                            ProjectTask *nt = realloc(tp->tasks, tp->tcap * sizeof(ProjectTask));
                            if (nt) tp->tasks = nt;
                        }
                        tp->tasks[tp->tcount] = fp->tasks[j];
                        tp->tcount++;
                        for (size_t k = j; k < fp->tcount - 1; k++) {
                            fp->tasks[k] = fp->tasks[k+1];
                        }
                        fp->tcount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *project = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, project) == 0) {
                    for (size_t j = 0; j < items[i].tcount; j++) {
                        if (strcmp(items[i].tasks[j].title, title) == 0) {
                            items[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    for (size_t j = 0; j < items[i].tcount; j++) {
                        free(items[i].tasks[j].title);
                        free(items[i].tasks[j].note);
                    }
                    free(items[i].tasks);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Project: %s\n", items[i].name);
                for (size_t j = 0; j < items[i].tcount; j++) {
                    printf("  Task: %s [%s] Status: %s\n", items[i].tasks[j].title, items[i].tasks[j].note, items[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        for (size_t j = 0; j < items[i].tcount; j++) {
            free(items[i].tasks[j].title);
            free(items[i].tasks[j].note);
        }
        free(items[i].tasks);
    }
    free(items);
    return 0;
}
