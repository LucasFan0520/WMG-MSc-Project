// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
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
    int task_count;
    int task_capacity;
} Project;

int main() {
    Project *projects = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("PROJECT %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("TASK %s %s [%s]\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    projects = realloc(projects, capacity * sizeof(Project));
                }
                projects[count].name = strdup(name);
                projects[count].tasks = NULL;
                projects[count].task_count = 0;
                projects[count].task_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char pname[256], title[256];
            int n2 = 0;
            if (sscanf(p, "%255s %255s%n", pname, title, &n2) == 2) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, pname) == 0) {
                        if (projects[i].task_count >= projects[i].task_capacity) {
                            projects[i].task_capacity = projects[i].task_capacity == 0 ? 4 : projects[i].task_capacity * 2;
                            projects[i].tasks = realloc(projects[i].tasks, projects[i].task_capacity * sizeof(Task));
                        }
                        projects[i].tasks[projects[i].task_count].title = strdup(title);
                        projects[i].tasks[projects[i].task_count].note = strdup(p);
                        projects[i].tasks[projects[i].task_count].done = 0;
                        projects[i].task_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char fromP[256], toP[256], title[256];
            if (sscanf(p, "%255s %255s %255s", fromP, toP, title) == 3) {
                int fIdx = -1, tIdx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, fromP) == 0) fIdx = i;
                    if (strcmp(projects[i].name, toP) == 0) tIdx = i;
                }
                if (fIdx != -1 && tIdx != -1) {
                    int found_task = -1;
                    for (int j = 0; j < projects[fIdx].task_count; j++) {
                        if (strcmp(projects[fIdx].tasks[j].title, title) == 0) {
                            found_task = j;
                            break;
                        }
                    }
                    if (found_task != -1) {
                        if (projects[tIdx].task_count >= projects[tIdx].task_capacity) {
                            projects[tIdx].task_capacity = projects[tIdx].task_capacity == 0 ? 4 : projects[tIdx].task_capacity * 2;
                            projects[tIdx].tasks = realloc(projects[tIdx].tasks, projects[tIdx].task_capacity * sizeof(Task));
                        }
                        projects[tIdx].tasks[projects[tIdx].task_count] = projects[fIdx].tasks[found_task];
                        projects[tIdx].task_count++;
                        for (int j = found_task; j < projects[fIdx].task_count - 1; j++) {
                            projects[fIdx].tasks[j] = projects[fIdx].tasks[j + 1];
                        }
                        projects[fIdx].task_count--;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char pname[256], title[256];
            if (sscanf(p, "%255s %255s", pname, title) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, pname) == 0) {
                        for (int j = 0; j < projects[i].task_count; j++) {
                            if (strcmp(projects[i].tasks[j].title, title) == 0) {
                                projects[i].tasks[j].done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(projects[found].name);
                    for (int j = 0; j < projects[found].task_count; j++) {
                        free(projects[found].tasks[j].title);
                        free(projects[found].tasks[j].note);
                    }
                    free(projects[found].tasks);
                    for (int i = found; i < count - 1; i++) {
                        projects[i] = projects[i + 1];
                    }
                    count--;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].task_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
