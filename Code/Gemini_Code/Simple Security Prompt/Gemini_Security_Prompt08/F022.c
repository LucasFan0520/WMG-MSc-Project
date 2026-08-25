// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
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

typedef struct {
    char *title;
    char *note;
    int done;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    size_t task_count;
    size_t task_cap;
} Project;

int main(void) {
    Project *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "PROJECT") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Project *nb = realloc(list, cap * sizeof(Project));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].name = strdup(arg1);
                list[count].tasks = NULL;
                list[count].task_count = 0;
                list[count].task_cap = 0;
                count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, proj) == 0) {
                    if (list[i].task_count >= list[i].task_cap) {
                        list[i].task_cap = list[i].task_cap == 0 ? 4 : list[i].task_cap * 2;
                        Task *nt = realloc(list[i].tasks, list[i].task_cap * sizeof(Task));
                        if (nt) list[i].tasks = nt;
                    }
                    if (list[i].task_count < list[i].task_cap) {
                        list[i].tasks[list[i].task_count].title = strdup(title);
                        list[i].tasks[list[i].task_count].note = strdup(note);
                        list[i].tasks[list[i].task_count].done = 0;
                        list[i].task_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromProj = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *toProj = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t f_idx = (size_t)-1;
            size_t t_idx = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, fromProj) == 0) f_idx = i;
                if (strcmp(list[i].name, toProj) == 0) t_idx = i;
            }
            if (f_idx != (size_t)-1 && t_idx != (size_t)-1) {
                for (size_t j = 0; j < list[f_idx].task_count; j++) {
                    if (strcmp(list[f_idx].tasks[j].title, title) == 0) {
                        if (list[t_idx].task_count >= list[t_idx].task_cap) {
                            list[t_idx].task_cap = list[t_idx].task_cap == 0 ? 4 : list[t_idx].task_cap * 2;
                            Task *nt = realloc(list[t_idx].tasks, list[t_idx].task_cap * sizeof(Task));
                            if (nt) list[t_idx].tasks = nt;
                        }
                        if (list[t_idx].task_count < list[t_idx].task_cap) {
                            list[t_idx].tasks[list[t_idx].task_count] = list[f_idx].tasks[j];
                            list[t_idx].task_count++;
                            for (size_t k = j; k < list[f_idx].task_count - 1; k++) {
                                list[f_idx].tasks[k] = list[f_idx].tasks[k + 1];
                            }
                            list[f_idx].task_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, proj) == 0) {
                    for (size_t j = 0; j < list[i].task_count; j++) {
                        if (strcmp(list[i].tasks[j].title, title) == 0) {
                            list[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    for (size_t j = 0; j < list[i].task_count; j++) {
                        free(list[i].tasks[j].title);
                        free(list[i].tasks[j].note);
                    }
                    free(list[i].tasks);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Project: %s\n", list[i].name);
                for (size_t j = 0; j < list[i].task_count; j++) {
                    printf("- [%s] %s: %s\n", list[i].tasks[j].done ? "X" : " ", list[i].tasks[j].title, list[i].tasks[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        for (size_t j = 0; j < list[i].task_count; j++) {
            free(list[i].tasks[j].title);
            free(list[i].tasks[j].note);
        }
        free(list[i].tasks);
    }
    free(list);
    return 0;
}
