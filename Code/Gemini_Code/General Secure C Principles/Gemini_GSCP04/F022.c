// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char *note;
    char *status;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    size_t task_count;
} Project;

int main(void) {
    Project *projects = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Project *new_projects = realloc(projects, capacity * sizeof(Project));
                    if (new_projects) projects = new_projects;
                }
                if (count < capacity) {
                    projects[count].name = strdup(name);
                    projects[count].tasks = NULL;
                    projects[count].task_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *pname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, pname) == 0) {
                    projects[i].tasks = realloc(projects[i].tasks, (projects[i].task_count + 1) * sizeof(Task));
                    projects[i].tasks[projects[i].task_count].title = strdup(title);
                    projects[i].tasks[projects[i].task_count].note = strdup(note);
                    projects[i].tasks[projects[i].task_count].status = strdup("TODO");
                    projects[i].task_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *fromP = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *toP = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long src_idx = -1, dest_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, fromP) == 0) src_idx = (long long)i;
                if (strcmp(projects[i].name, toP) == 0) dest_idx = (long long)i;
            }
            if (src_idx != -1 && dest_idx != -1 && src_idx != dest_idx) {
                for (size_t k = 0; k < projects[src_idx].task_count; k++) {
                    if (strcmp(projects[src_idx].tasks[k].title, title) == 0) {
                        projects[dest_idx].tasks = realloc(projects[dest_idx].tasks, (projects[dest_idx].task_count + 1) * sizeof(Task));
                        projects[dest_idx].tasks[projects[dest_idx].task_count] = projects[src_idx].tasks[k];
                        projects[dest_idx].task_count++;
                        for (size_t j = k; j < projects[src_idx].task_count - 1; j++) {
                            projects[src_idx].tasks[j] = projects[src_idx].tasks[j + 1];
                        }
                        projects[src_idx].task_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *pname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, pname) == 0) {
                    for (size_t k = 0; k < projects[i].task_count; k++) {
                        if (strcmp(projects[i].tasks[k].title, title) == 0) {
                            free(projects[i].tasks[k].status);
                            projects[i].tasks[k].status = strdup("DONE");
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (size_t k = 0; k < projects[i].task_count; k++) {
                        free(projects[i].tasks[k].title);
                        free(projects[i].tasks[k].note);
                        free(projects[i].tasks[k].status);
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
                printf("PROJECT: %s\n", projects[i].name);
                for (size_t k = 0; k < projects[i].task_count; k++) {
                    printf("  [%s] %s - %s\n", projects[i].tasks[k].status, projects[i].tasks[k].title, projects[i].tasks[k].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(projects[i].name);
        for (size_t k = 0; k < projects[i].task_count; k++) {
            free(projects[i].tasks[k].title);
            free(projects[i].tasks[k].note);
            free(projects[i].tasks[k].status);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
