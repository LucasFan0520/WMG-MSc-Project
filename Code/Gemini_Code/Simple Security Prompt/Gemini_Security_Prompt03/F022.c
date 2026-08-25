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
    int task_count;
} Project;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

int main(void) {
    Project *projects = NULL;
    int project_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            projects = realloc(projects, (project_count + 1) * sizeof(Project));
            projects[project_count].name = strdup(name);
            projects[project_count].tasks = NULL;
            projects[project_count].task_count = 0;
            project_count++;
        } else if (strcmp(cmd, "TASK") == 0) {
            while (*p == ' ') p++;
            char *pname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            for (int i = 0; i < project_count; i++) {
                if (projects[i].name && strcmp(projects[i].name, pname) == 0) {
                    int tc = projects[i].task_count;
                    projects[i].tasks = realloc(projects[i].tasks, (tc + 1) * sizeof(Task));
                    projects[i].tasks[tc].title = strdup(title);
                    projects[i].tasks[tc].note = strdup(note);
                    projects[i].tasks[tc].done = 0;
                    projects[i].task_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            while (*p == ' ') p++;
            char *fromP = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *toP = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idxFrom = -1, idxTo = -1;
            for (int i = 0; i < project_count; i++) {
                if (projects[i].name && strcmp(projects[i].name, fromP) == 0) idxFrom = i;
                if (projects[i].name && strcmp(projects[i].name, toP) == 0) idxTo = i;
            }
            if (idxFrom != -1 && idxTo != -1) {
                for (int j = 0; j < projects[idxFrom].task_count; j++) {
                    if (strcmp(projects[idxFrom].tasks[j].title, title) == 0) {
                        int tcTo = projects[idxTo].task_count;
                        projects[idxTo].tasks = realloc(projects[idxTo].tasks, (tcTo + 1) * sizeof(Task));
                        projects[idxTo].tasks[tcTo] = projects[idxFrom].tasks[j];
                        projects[idxTo].task_count++;
                        for (int k = j; k < projects[idxFrom].task_count - 1; k++) {
                            projects[idxFrom].tasks[k] = projects[idxFrom].tasks[k + 1];
                        }
                        projects[idxFrom].task_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            while (*p == ' ') p++;
            char *pname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < project_count; i++) {
                if (projects[i].name && strcmp(projects[i].name, pname) == 0) {
                    for (int j = 0; j < projects[i].task_count; j++) {
                        if (strcmp(projects[i].tasks[j].title, title) == 0) {
                            projects[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            while (*p == ' ') p++;
            char *pname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < project_count; i++) {
                if (projects[i].name && strcmp(projects[i].name, pname) == 0) {
                    free(projects[i].name);
                    for (int j = 0; j < projects[i].task_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    projects[i].name = NULL;
                    projects[i].tasks = NULL;
                    projects[i].task_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < project_count; i++) {
                if (projects[i].name) {
                    printf("Project: %s\n", projects[i].name);
                    for (int j = 0; j < projects[i].task_count; j++) {
                        printf("  Task: %s [%s] (%s)\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < project_count; i++) {
        if (projects[i].name) {
            free(projects[i].name);
            for (int j = 0; j < projects[i].task_count; j++) {
                free(projects[i].tasks[j].title);
                free(projects[i].tasks[j].note);
            }
            free(projects[i].tasks);
        }
    }
    free(projects);
    return 0;
}
