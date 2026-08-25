// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* title; char* note; int done; } Task;
typedef struct { char* name; Task* tasks; size_t t_count, t_cap; } Project;

Project* projects = NULL;
size_t p_count = 0, p_cap = 0;

void cleanup() {
    for (size_t i = 0; i < p_count; i++) {
        for (size_t j = 0; j < projects[i].t_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
        free(projects[i].name);
    }
    free(projects);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "PROJECT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            if (!*ptr) continue;
            if (p_count == p_cap) {
                size_t new_cap = p_cap == 0 ? 4 : p_cap * 2;
                Project* temp = realloc(projects, new_cap * sizeof(Project));
                if (!temp) { cleanup(); free(line); return 1; }
                projects = temp; p_cap = new_cap;
            }
            projects[p_count].name = strdup(ptr);
            projects[p_count].tasks = NULL;
            projects[p_count].t_count = 0;
            projects[p_count].t_cap = 0;
            if (!projects[p_count].name) { cleanup(); free(line); return 1; }
            p_count++;
        } else if (strcmp(cmd, "TASK") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* pname = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;

            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, pname) == 0) {
                    if (projects[i].t_count == projects[i].t_cap) {
                        size_t new_cap = projects[i].t_cap == 0 ? 4 : projects[i].t_cap * 2;
                        Task* temp = realloc(projects[i].tasks, new_cap * sizeof(Task));
                        if (!temp) { cleanup(); free(line); return 1; }
                        projects[i].tasks = temp; projects[i].t_cap = new_cap;
                    }
                    projects[i].tasks[projects[i].t_count].title = strdup(title);
                    projects[i].tasks[projects[i].t_count].note = strdup(note);
                    projects[i].tasks[projects[i].t_count].done = 0;
                    if (!projects[i].tasks[projects[i].t_count].title || !projects[i].tasks[projects[i].t_count].note) {
                        cleanup(); free(line); return 1;
                    }
                    projects[i].t_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* from = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* to = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;

            Project* pf = NULL, *pt = NULL;
            Task* t = NULL;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, from) == 0) pf = &projects[i];
                if (strcmp(projects[i].name, to) == 0) pt = &projects[i];
            }
            if (pf && pt) {
                for (size_t i = 0; i < pf->t_count; i++) {
                    if (strcmp(pf->tasks[i].title, title) == 0) {
                        t = &pf->tasks[i];
                        break;
                    }
                }
                if (t) {
                    if (pt->t_count == pt->t_cap) {
                        size_t new_cap = pt->t_cap == 0 ? 4 : pt->t_cap * 2;
                        Task* temp = realloc(pt->tasks, new_cap * sizeof(Task));
                        if (!temp) { cleanup(); free(line); return 1; }
                        pt->tasks = temp; pt->t_cap = new_cap;
                    }
                    pt->tasks[pt->t_count] = *t;
                    pt->t_count++;
                    for (size_t i = 0; i < pf->t_count - 1; i++) {
                        if (&pf->tasks[i] == t) {
                            for (size_t j = i; j < pf->t_count - 1; j++) pf->tasks[j] = pf->tasks[j + 1];
                            break;
                        }
                    }
                    pf->t_count--;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* pname = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;

            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, pname) == 0) {
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
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, ptr) == 0) {
                    for (size_t j = 0; j < projects[i].t_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    free(projects[i].name);
                    for (size_t j = i; j < p_count - 1; j++) projects[j] = projects[j + 1];
                    p_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < p_count; i++) {
                printf("%s\n", projects[i].name);
                for (size_t j = 0; j < projects[i].t_count; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}