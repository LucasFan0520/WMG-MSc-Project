// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

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

int main() {
    Project *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = get_token(&p);
            if (name) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Project *narr = realloc(arr, cap * sizeof(Project));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].tasks = NULL;
                arr[count].tcount = 0;
                arr[count].tcap = 0;
                count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = get_token(&p);
            char *title = get_token(&p);
            char *note = get_rest(&p);
            if (proj && title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, proj) == 0) {
                        Project *pr = &arr[i];
                        if (pr->tcount >= pr->tcap) {
                            pr->tcap = pr->tcap == 0 ? 4 : pr->tcap * 2;
                            Task *ntasks = realloc(pr->tasks, pr->tcap * sizeof(Task));
                            if (ntasks) pr->tasks = ntasks;
                        }
                        pr->tasks[pr->tcount].title = strdup(title);
                        pr->tasks[pr->tcount].note = strdup(note);
                        pr->tasks[pr->tcount].done = 0;
                        pr->tcount++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromP = get_token(&p);
            char *toP = get_token(&p);
            char *title = get_token(&p);
            if (fromP && toP && title) {
                int fidx = -1, tidx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, fromP) == 0) fidx = i;
                    if (strcmp(arr[i].name, toP) == 0) tidx = i;
                }
                if (fidx != -1 && tidx != -1) {
                    for (int j = 0; j < arr[fidx].tcount; j++) {
                        if (strcmp(arr[fidx].tasks[j].title, title) == 0) {
                            Project *dest = &arr[tidx];
                            if (dest->tcount >= dest->tcap) {
                                dest->tcap = dest->tcap == 0 ? 4 : dest->tcap * 2;
                                Task *ntasks = realloc(dest->tasks, dest->tcap * sizeof(Task));
                                if (ntasks) dest->tasks = ntasks;
                            }
                            dest->tasks[dest->tcount] = arr[fidx].tasks[j];
                            dest->tcount++;
                            for (int k = j; k < arr[fidx].tcount - 1; k++) {
                                arr[fidx].tasks[k] = arr[fidx].tasks[k + 1];
                            }
                            arr[fidx].tcount--;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = get_token(&p);
            char *title = get_token(&p);
            if (proj && title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, proj) == 0) {
                        for (int j = 0; j < arr[i].tcount; j++) {
                            if (strcmp(arr[i].tasks[j].title, title) == 0) {
                                arr[i].tasks[j].done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        for (int j = 0; j < arr[i].tcount; j++) {
                            free(arr[i].tasks[j].title);
                            free(arr[i].tasks[j].note);
                        }
                        free(arr[i].tasks);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
                for (int j = 0; j < arr[i].tcount; j++) {
                    printf("%s %s %s\n", arr[i].tasks[j].title, arr[i].tasks[j].note, arr[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].tcount; j++) {
            free(arr[i].tasks[j].title);
            free(arr[i].tasks[j].note);
        }
        free(arr[i].tasks);
    }
    free(arr);
    return 0;
}
