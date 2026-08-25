// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

typedef struct {
    char *title;
    char *note;
    int done;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    size_t tcount;
    size_t tcap;
} Project;

int main() {
    Project *projects = NULL;
    size_t pcount = 0;
    size_t pcap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            if (pcount >= pcap) {
                pcap = pcap == 0 ? 4 : pcap * 2;
                projects = realloc(projects, pcap * sizeof(Project));
            }
            projects[pcount].name = strdup(name);
            projects[pcount].tasks = NULL;
            projects[pcount].tcount = 0;
            projects[pcount].tcap = 0;
            pcount++;
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p = line + 5;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *proj_name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                char *title = p2;
                char *note = "";
                if (s2) {
                    *s2 = '\0';
                    note = s2 + 1;
                }
                for (size_t i = 0; i < pcount; i++) {
                    if (strcmp(projects[i].name, proj_name) == 0) {
                        Project *pr = &projects[i];
                        if (pr->tcount >= pr->tcap) {
                            pr->tcap = pr->tcap == 0 ? 4 : pr->tcap * 2;
                            pr->tasks = realloc(pr->tasks, pr->tcap * sizeof(Task));
                        }
                        pr->tasks[pr->tcount].title = strdup(title);
                        pr->tasks[pr->tcount].note = strdup(note);
                        pr->tasks[pr->tcount].done = 0;
                        pr->tcount++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p = line + 9;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *from_proj = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *to_proj = p2;
                    char *title = s2 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (size_t i = 0; i < pcount; i++) {
                        if (strcmp(projects[i].name, from_proj) == 0) f_idx = (int)i;
                        if (strcmp(projects[i].name, to_proj) == 0) t_idx = (int)i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        Project *fp = &projects[f_idx];
                        Project *tp = &projects[t_idx];
                        for (size_t j = 0; j < fp->tcount; j++) {
                            if (strcmp(fp->tasks[j].title, title) == 0) {
                                if (tp->tcount >= tp->tcap) {
                                    tp->tcap = tp->tcap == 0 ? 4 : tp->tcap * 2;
                                    tp->tasks = realloc(tp->tasks, tp->tcap * sizeof(Task));
                                }
                                tp->tasks[tp->tcount] = fp->tasks[j];
                                tp->tcount++;
                                for (size_t k = j; k < fp->tcount - 1; k++) {
                                    fp->tasks[k] = fp->tasks[k + 1];
                                }
                                fp->tcount--;
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p = line + 9;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *proj_name = p;
                char *title = s1 + 1;
                for (size_t i = 0; i < pcount; i++) {
                    if (strcmp(projects[i].name, proj_name) == 0) {
                        for (size_t j = 0; j < projects[i].tcount; j++) {
                            if (strcmp(projects[i].tasks[j].title, title) == 0) {
                                projects[i].tasks[j].done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
            for (size_t i = 0; i < pcount; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (size_t j = 0; j < projects[i].tcount; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (size_t j = i; j < pcount - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    pcount--;
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < pcount; i++) {
                printf("Project %s:\n", projects[i].name);
                for (size_t j = 0; j < projects[i].tcount; j++) {
                    printf("- [%s] %s: %s\n", projects[i].tasks[j].done ? "X" : " ", projects[i].tasks[j].title, projects[i].tasks[j].note);
                }
            }
        }
        free(line);
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
