// F022.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int done;
} ProjectTask;

typedef struct {
    char *name;
    ProjectTask *tasks;
    size_t task_count;
    size_t task_capacity;
} Project;

int main(void) {
    Project *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            while (*name == ' ') name++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Project *tmp = realloc(arr, capacity * sizeof(Project));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                if (!arr[count].name) exit(1);
                arr[count].tasks = NULL;
                arr[count].task_count = 0;
                arr[count].task_capacity = 0;
                count++;
            }
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *proj = p;
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                title = p;
            }
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, proj) == 0) {
                    Project *r = &arr[i];
                    if (r->task_count >= r->task_capacity) {
                        r->task_capacity = r->task_capacity == 0 ? 4 : r->task_capacity * 2;
                        ProjectTask *tmp = realloc(r->tasks, r->task_capacity * sizeof(ProjectTask));
                        if (!tmp) exit(1);
                        r->tasks = tmp;
                    }
                    r->tasks[r->task_count].title = strdup(title);
                    r->tasks[r->task_count].note = strdup(note);
                    r->tasks[r->task_count].done = 0;
                    if (!r->tasks[r->task_count].title || !r->tasks[r->task_count].note) exit(1);
                    r->task_count++;
                    break;
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *fromProj = p;
            while (*p && *p != ' ') p++;
            char *toProj = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                toProj = p;
            }
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                title = p;
            }
            int idxFrom = -1, idxTo = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, fromProj) == 0) idxFrom = (int)i;
                if (strcmp(arr[i].name, toProj) == 0) idxTo = (int)i;
            }
            if (idxFrom != -1 && idxTo != -1) {
                Project *f = &arr[idxFrom];
                Project *t = &arr[idxTo];
                for (size_t j = 0; j < f->task_count; j++) {
                    if (strcmp(f->tasks[j].title, title) == 0) {
                        if (t->task_count >= t->task_capacity) {
                            t->task_capacity = t->task_capacity == 0 ? 4 : t->task_capacity * 2;
                            ProjectTask *tmp = realloc(t->tasks, t->task_capacity * sizeof(ProjectTask));
                            if (!tmp) exit(1);
                            t->tasks = tmp;
                        }
                        t->tasks[t->task_count] = f->tasks[j];
                        t->task_count++;
                        for (size_t m = j; m < f->task_count - 1; m++) {
                            f->tasks[m] = f->tasks[m + 1];
                        }
                        f->task_count--;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *proj = p;
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                title = p;
            }
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
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
            while (*name == ' ') name++;
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
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Project: %s\n", arr[i].name);
                for (size_t j = 0; j < arr[i].task_count; j++) {
                    printf("  - [%s] %s: %s\n", arr[i].tasks[j].done ? "X" : " ", arr[i].tasks[j].title, arr[i].tasks[j].note);
                }
            }
        }
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
    free(line);
    return 0;
}
