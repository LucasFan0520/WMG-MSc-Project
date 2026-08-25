// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
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
    size_t task_capacity;
} Project;

int main(void) {
    Project *projects = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Project *new_p = realloc(projects, capacity * sizeof(Project));
                    if (new_p) projects = new_p;
                }
                projects[count].name = mystrdup(name);
                projects[count].tasks = NULL;
                projects[count].task_count = 0;
                projects[count].task_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = p;
            while (*proj == ' ') proj++;
            char *proj_end = proj;
            while (*proj_end && *proj_end != ' ') proj_end++;
            char *title = proj_end;
            if (*proj_end != '\0') {
                *proj_end = '\0';
                title++;
            }
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *note = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    Project *pr = &projects[i];
                    if (pr->task_count >= pr->task_capacity) {
                        pr->task_capacity = pr->task_capacity == 0 ? 4 : pr->task_capacity * 2;
                        ProjectTask *new_t = realloc(pr->tasks, pr->task_capacity * sizeof(ProjectTask));
                        if (new_t) pr->tasks = new_t;
                    }
                    pr->tasks[pr->task_count].title = mystrdup(title);
                    pr->tasks[pr->task_count].note = mystrdup(note);
                    pr->tasks[pr->task_count].done = 0;
                    pr->task_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromP = p;
            while (*fromP == ' ') fromP++;
            char *fromP_end = fromP;
            while (*fromP_end && *fromP_end != ' ') fromP_end++;
            char *toP = fromP_end;
            if (*fromP_end != '\0') {
                *fromP_end = '\0';
                toP++;
            }
            while (*toP == ' ') toP++;
            char *toP_end = toP;
            while (*toP_end && *toP_end != ' ') toP_end++;
            char *title = toP_end;
            if (*toP_end != '\0') {
                *toP_end = '\0';
                title++;
            }
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            int f_idx = -1, t_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, fromP) == 0) f_idx = (int)i;
                if (strcmp(projects[i].name, toP) == 0) t_idx = (int)i;
            }
            if (f_idx != -1 && t_idx != -1) {
                Project *fp_ptr = &projects[f_idx];
                Project *tp_ptr = &projects[t_idx];
                for (size_t j = 0; j < fp_ptr->task_count; j++) {
                    if (strcmp(fp_ptr->tasks[j].title, title) == 0) {
                        if (tp_ptr->task_count >= tp_ptr->task_capacity) {
                            tp_ptr->task_capacity = tp_ptr->task_capacity == 0 ? 4 : tp_ptr->task_capacity * 2;
                            ProjectTask *new_t = realloc(tp_ptr->tasks, tp_ptr->task_capacity * sizeof(ProjectTask));
                            if (new_t) tp_ptr->tasks = new_t;
                        }
                        tp_ptr->tasks[tp_ptr->task_count] = fp_ptr->tasks[j];
                        tp_ptr->task_count++;
                        for (size_t k = j; k < fp_ptr->task_count - 1; k++) {
                            fp_ptr->tasks[k] = fp_ptr->tasks[k + 1];
                        }
                        fp_ptr->task_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = p;
            while (*proj == ' ') proj++;
            char *proj_end = proj;
            while (*proj_end && *proj_end != ' ') proj_end++;
            char *title = proj_end;
            if (*proj_end != '\0') {
                *proj_end = '\0';
                title++;
            }
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    for (size_t j = 0; j < projects[i].task_count; j++) {
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
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (size_t j = 0; j < projects[i].task_count; j++) {
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
                for (size_t j = 0; j < projects[i].task_count; j++) {
                    printf("TASK %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(projects[i].name);
        for (size_t j = 0; j < projects[i].task_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
