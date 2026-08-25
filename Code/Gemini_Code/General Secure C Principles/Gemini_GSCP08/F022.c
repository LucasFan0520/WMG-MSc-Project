// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
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
} ProjectTask;

typedef struct {
    char *name;
    ProjectTask *tasks;
    size_t task_count;
    size_t task_cap;
} Project;

int main(void) {
    Project *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("PROJECT %s\n", arr[i].name);
                    for (size_t j = 0; j < arr[i].task_count; j++) {
                        printf("TASK %s %s %s\n", arr[i].tasks[j].title, arr[i].tasks[j].done ? "DONE" : "TODO", arr[i].tasks[j].note);
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = args;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    Project *narr = realloc(arr, ncap * sizeof(Project));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].name = strdup(name);
                    arr[count].tasks = NULL;
                    arr[count].task_count = 0;
                    arr[count].task_cap = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = args;
            char *p2 = strchr(proj, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = p2 + 1;
                char *p3 = strchr(title, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, proj) == 0) {
                            Project *p = &arr[i];
                            if (p->task_count >= p->task_cap) {
                                size_t ncap = p->task_cap == 0 ? 4 : p->task_cap * 2;
                                ProjectTask *ntasks = realloc(p->tasks, ncap * sizeof(ProjectTask));
                                if (ntasks) {
                                    p->tasks = ntasks;
                                    p->task_cap = ncap;
                                }
                            }
                            if (p->task_count < p->task_cap) {
                                p->tasks[p->task_count].title = strdup(title);
                                p->tasks[p->task_count].note = strdup(note);
                                p->tasks[p->task_count].done = 0;
                                p->task_count++;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromP = args;
            char *p2 = strchr(fromP, ' ');
            if (p2) {
                *p2 = '\0';
                char *toP = p2 + 1;
                char *p3 = strchr(toP, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *title = p3 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, fromP) == 0) f_idx = (int)i;
                        if (strcmp(arr[i].name, toP) == 0) t_idx = (int)i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        Project *fp = &arr[f_idx];
                        Project *tp = &arr[t_idx];
                        for (size_t j = 0; j < fp->task_count; j++) {
                            if (strcmp(fp->tasks[j].title, title) == 0) {
                                if (tp->task_count >= tp->task_cap) {
                                    size_t ncap = tp->task_cap == 0 ? 4 : tp->task_cap * 2;
                                    ProjectTask *ntasks = realloc(tp->tasks, ncap * sizeof(ProjectTask));
                                    if (ntasks) {
                                        tp->tasks = ntasks;
                                        tp->task_cap = ncap;
                                    }
                                }
                                if (tp->task_count < tp->task_cap) {
                                    tp->tasks[tp->task_count] = fp->tasks[j];
                                    tp->task_count++;
                                    for (size_t k = j; k < fp->task_count - 1; k++) {
                                        fp->tasks[k] = fp->tasks[k + 1];
                                    }
                                    fp->task_count--;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = args;
            char *p2 = strchr(proj, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, proj) == 0) {
                        for (size_t j = 0; j < arr[i].task_count; j++) {
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
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (size_t j = 0; j < arr[i].task_count; j++) {
                        free(arr[i].tasks[j].title);
                        free(arr[i].tasks[j].note);
                    }
                    free(arr[i].tasks);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        for (size_t j = 0; j < arr[i].task_count; j++) {
            free(arr[i].tasks[j].title);
            free(arr[i].tasks[j].note);
        }
        free(arr[i].tasks);
    }
    free(arr);
    return 0;
}
