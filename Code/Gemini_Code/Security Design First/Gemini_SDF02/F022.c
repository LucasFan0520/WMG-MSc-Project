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
    size_t t_count;
    size_t t_capacity;
} Project;

int main(void) {
    Project *projects = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Project *temp = realloc(projects, new_cap * sizeof(Project));
                    if (!temp) break;
                    projects = temp;
                    capacity = new_cap;
                }
                projects[count].name = strdup(name);
                projects[count].tasks = NULL;
                projects[count].t_count = 0;
                projects[count].t_capacity = 0;
                if (projects[count].name) count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    Project *pr = &projects[i];
                    if (pr->t_count >= pr->t_capacity) {
                        size_t new_cap = pr->t_capacity == 0 ? 4 : pr->t_capacity * 2;
                        Task *temp = realloc(pr->tasks, new_cap * sizeof(Task));
                        if (!temp) break;
                        pr->tasks = temp;
                        pr->t_capacity = new_cap;
                    }
                    pr->tasks[pr->t_count].title = strdup(title);
                    pr->tasks[pr->t_count].note = strdup(note);
                    pr->tasks[pr->t_count].done = 0;
                    if (pr->tasks[pr->t_count].title && pr->tasks[pr->t_count].note) {
                        pr->t_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromProj = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *toProj = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;

            Project *fp = NULL;
            Project *tp = NULL;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, fromProj) == 0) fp = &projects[i];
                if (strcmp(projects[i].name, toProj) == 0) tp = &projects[i];
            }
            if (fp && tp) {
                for (size_t j = 0; j < fp->t_count; j++) {
                    if (strcmp(fp->tasks[j].title, title) == 0) {
                        if (tp->t_count >= tp->t_capacity) {
                            size_t new_cap = tp->t_capacity == 0 ? 4 : tp->t_capacity * 2;
                            Task *temp = realloc(tp->tasks, new_cap * sizeof(Task));
                            if (!temp) break;
                            tp->tasks = temp;
                            tp->t_capacity = new_cap;
                        }
                        tp->tasks[tp->t_count] = fp->tasks[j];
                        tp->t_count++;
                        for (size_t k = j; k < fp->t_count - 1; k++) {
                            fp->tasks[k] = fp->tasks[k + 1];
                        }
                        fp->t_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    for (size_t j = 0; j < projects[i].t_count; j++) {
                        if (strcmp(projects[i].tasks[j].title, title) == 0) {
                            projects[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (size_t j = 0; j < projects[i].t_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (size_t j = i; j < count - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("PROJECT %s\n", projects[i].name);
                for (size_t j = 0; j < projects[i].t_count; j++) {
                    printf("TASK %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(projects[i].name);
        for (size_t j = 0; j < projects[i].t_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
