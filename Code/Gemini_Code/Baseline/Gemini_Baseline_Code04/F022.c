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

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Project *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            arr = realloc(arr, (count + 1) * sizeof(Project));
            arr[count].name = strdup(name);
            arr[count].tasks = NULL;
            arr[count].task_count = 0;
            count++;
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p = line + 5;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *title = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, proj) == 0) {
                            int tc = arr[i].task_count;
                            arr[i].tasks = realloc(arr[i].tasks, (tc + 1) * sizeof(Task));
                            arr[i].tasks[tc].title = strdup(title);
                            arr[i].tasks[tc].note = strdup(note);
                            arr[i].tasks[tc].done = 0;
                            arr[i].task_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p = line + 9;
            char *fromProj = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *toProj = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *title = p + 1;
                    int idxFrom = -1, idxTo = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, fromProj) == 0) idxFrom = i;
                        if (strcmp(arr[i].name, toProj) == 0) idxTo = i;
                    }
                    if (idxFrom != -1 && idxTo != -1) {
                        for (int j = 0; j < arr[idxFrom].task_count; j++) {
                            if (strcmp(arr[idxFrom].tasks[j].title, title) == 0) {
                                int tc = arr[idxTo].task_count;
                                arr[idxTo].tasks = realloc(arr[idxTo].tasks, (tc + 1) * sizeof(Task));
                                arr[idxTo].tasks[tc] = arr[idxFrom].tasks[j];
                                arr[idxTo].task_count++;
                                for (int k = j; k < arr[idxFrom].task_count - 1; k++) {
                                    arr[idxFrom].tasks[k] = arr[idxFrom].tasks[k + 1];
                                }
                                arr[idxFrom].task_count--;
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p = line + 9;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *title = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, proj) == 0) {
                        for (int j = 0; j < arr[i].task_count; j++) {
                            if (strcmp(arr[i].tasks[j].title, title) == 0) {
                                arr[i].tasks[j].done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (int j = 0; j < arr[i].task_count; j++) {
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
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("PROJECT %s\n", arr[i].name);
                for (int j = 0; j < arr[i].task_count; j++) {
                    printf("TASK %s %s %s\n", arr[i].tasks[j].title, arr[i].tasks[j].done ? "DONE" : "TODO", arr[i].tasks[j].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].task_count; j++) {
            free(arr[i].tasks[j].title);
            free(arr[i].tasks[j].note);
        }
        free(arr[i].tasks);
    }
    free(arr);
    return 0;
}
