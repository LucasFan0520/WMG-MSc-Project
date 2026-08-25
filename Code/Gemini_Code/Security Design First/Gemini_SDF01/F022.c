// F022.c
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
    size_t task_count;
    size_t task_capacity;
} Project;

int main(void) {
    Project *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found == -1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Project *new_items = realloc(items, capacity * sizeof(Project));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                if (!items[count].name) exit(1);
                items[count].tasks = NULL;
                items[count].task_count = 0;
                items[count].task_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *project = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, project) == 0) {
                    Project *prj = &items[i];
                    if (prj->task_count >= prj->task_capacity) {
                        prj->task_capacity = prj->task_capacity == 0 ? 4 : prj->task_capacity * 2;
                        Task *new_tasks = realloc(prj->tasks, prj->task_capacity * sizeof(Task));
                        if (!new_tasks) exit(1);
                        prj->tasks = new_tasks;
                    }
                    prj->tasks[prj->task_count].title = strdup(title);
                    prj->tasks[prj->task_count].note = strdup(note);
                    prj->tasks[prj->task_count].done = 0;
                    if (!prj->tasks[prj->task_count].title || !prj->tasks[prj->task_count].note) exit(1);
                    prj->task_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromProj = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *toProj = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int idxFrom = -1, idxTo = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, fromProj) == 0) idxFrom = (int)i;
                if (strcmp(items[i].name, toProj) == 0) idxTo = (int)i;
            }

            if (idxFrom != -1 && idxTo != -1) {
                Project *pf = &items[idxFrom];
                Project *pt = &items[idxTo];
                for (size_t j = 0; j < pf->task_count; j++) {
                    if (strcmp(pf->tasks[j].title, title) == 0) {
                        if (pt->task_count >= pt->task_capacity) {
                            pt->task_capacity = pt->task_capacity == 0 ? 4 : pt->task_capacity * 2;
                            Task *new_tasks = realloc(pt->tasks, pt->task_capacity * sizeof(Task));
                            if (!new_tasks) exit(1);
                            pt->tasks = new_tasks;
                        }
                        pt->tasks[pt->task_count] = pf->tasks[j];
                        pt->task_count++;

                        for (size_t k = j; k < pf->task_count - 1; k++) {
                            pf->tasks[k] = pf->tasks[k + 1];
                        }
                        pf->task_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *project = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, project) == 0) {
                    for (size_t j = 0; j < items[i].task_count; j++) {
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
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    for (size_t j = 0; j < items[i].task_count; j++) {
                        free(items[i].tasks[j].title);
                        free(items[i].tasks[j].note);
                    }
                    free(items[i].tasks);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Project: %s\n", items[i].name);
                for (size_t j = 0; j < items[i].task_count; j++) {
                    printf("  Task: %s [%s] - %s\n", items[i].tasks[j].title, items[i].tasks[j].done ? "DONE" : "TODO", items[i].tasks[j].note);
                }
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        for (size_t j = 0; j < items[i].task_count; j++) {
            free(items[i].tasks[j].title);
            free(items[i].tasks[j].note);
        }
        free(items[i].tasks);
    }
    free(items);
    free(line);
    return 0;
}
