// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
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
    int task_cap;
} Project;

int main() {
    Project *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Project *narr = realloc(arr, cap * sizeof(Project));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].name = mystrdup(name);
                    arr[count].tasks = NULL;
                    arr[count].task_count = 0;
                    arr[count].task_cap = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *project = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, project) == 0) {
                    if (arr[i].task_count >= arr[i].task_cap) {
                        arr[i].task_cap = arr[i].task_cap == 0 ? 4 : arr[i].task_cap * 2;
                        Task *nt = realloc(arr[i].tasks, arr[i].task_cap * sizeof(Task));
                        if (nt) arr[i].tasks = nt;
                    }
                    if (arr[i].task_count < arr[i].task_cap) {
                        arr[i].tasks[arr[i].task_count].title = mystrdup(title);
                        arr[i].tasks[arr[i].task_count].note = mystrdup(note);
                        arr[i].tasks[arr[i].task_count].done = 0;
                        arr[i].task_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *fromProj = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *toProj = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int f_idx = -1, t_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, fromProj) == 0) f_idx = i;
                if (strcmp(arr[i].name, toProj) == 0) t_idx = i;
            }
            if (f_idx != -1 && t_idx != -1) {
                for (int j = 0; j < arr[f_idx].task_count; j++) {
                    if (strcmp(arr[f_idx].tasks[j].title, title) == 0) {
                        if (arr[t_idx].task_count >= arr[t_idx].task_cap) {
                            arr[t_idx].task_cap = arr[t_idx].task_cap == 0 ? 4 : arr[t_idx].task_cap * 2;
                            Task *nt = realloc(arr[t_idx].tasks, arr[t_idx].task_cap * sizeof(Task));
                            if (nt) arr[t_idx].tasks = nt;
                        }
                        if (arr[t_idx].task_count < arr[t_idx].task_cap) {
                            arr[t_idx].tasks[arr[t_idx].task_count] = arr[f_idx].tasks[j];
                            arr[t_idx].task_count++;
                            for (int k = j; k < arr[f_idx].task_count - 1; k++) {
                                arr[f_idx].tasks[k] = arr[f_idx].tasks[k + 1];
                            }
                            arr[f_idx].task_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *project = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, project) == 0) {
                    for (int j = 0; j < arr[i].task_count; j++) {
                        if (strcmp(arr[i].tasks[j].title, title) == 0) {
                            arr[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
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
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Project: %s\n", arr[i].name);
                for (int j = 0; j < arr[i].task_count; j++) {
                    printf("  Task: %s [%s] - %s\n", arr[i].tasks[j].title, arr[i].tasks[j].done ? "DONE" : "TODO", arr[i].tasks[j].note);
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
