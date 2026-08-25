// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
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

int main(void) {
    Project *projects = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name_start = line + 8;
            while (*name_start == ' ') name_start++;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name_start) == 0) {
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
                if (count < capacity) {
                    projects[count].name = strdup(name_start);
                    projects[count].tasks = NULL;
                    projects[count].task_count = 0;
                    projects[count].task_capacity = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *p_name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *title = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, p_name) == 0) {
                            Project *proj = &projects[i];
                            if (proj->task_count >= proj->task_capacity) {
                                proj->task_capacity = proj->task_capacity == 0 ? 4 : proj->task_capacity * 2;
                                Task *new_t = realloc(proj->tasks, proj->task_capacity * sizeof(Task));
                                if (new_t) proj->tasks = new_t;
                            }
                            if (proj->task_count < proj->task_capacity) {
                                proj->tasks[proj->task_count].title = strdup(title);
                                proj->tasks[proj->task_count].note = strdup(note);
                                proj->tasks[proj->task_count].done = 0;
                                proj->task_count++;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *from_p = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *to_p = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *title = p + 1;
                    while (*title == ' ') title++;
                    int idx_from = -1, idx_to = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, from_p) == 0) idx_from = i;
                        if (strcmp(projects[i].name, to_p) == 0) idx_to = i;
                    }
                    if (idx_from != -1 && idx_to != -1) {
                        Project *pf = &projects[idx_from];
                        Project *pt = &projects[idx_to];
                        for (int j = 0; j < pf->task_count; j++) {
                            if (strcmp(pf->tasks[j].title, title) == 0) {
                                if (pt->task_count >= pt->task_capacity) {
                                    pt->task_capacity = pt->task_capacity == 0 ? 4 : pt->task_capacity * 2;
                                    Task *new_t = realloc(pt->tasks, pt->task_capacity * sizeof(Task));
                                    if (new_t) pt->tasks = new_t;
                                }
                                if (pt->task_count < pt->task_capacity) {
                                    pt->tasks[pt->task_count] = pf->tasks[j];
                                    pt->task_count++;
                                    for (int k = j; k < pf->task_count - 1; k++) {
                                        pf->tasks[k] = pf->tasks[k + 1];
                                    }
                                    pf->task_count--;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *p_name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *title = p + 1;
                while (*title == ' ') title++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, p_name) == 0) {
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
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name_start = line + 14;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name_start) == 0) {
                    free(projects[i].name);
                    for (int j = 0; j < projects[i].task_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (int j = i; j < count - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("  [%s] %s: %s\n", projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].title, projects[i].tasks[j].note);
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
