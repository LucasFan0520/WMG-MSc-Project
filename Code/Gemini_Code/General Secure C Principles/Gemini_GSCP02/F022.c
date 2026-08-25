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
    int tcount;
} Project;

int main(void) {
    Project *projects = NULL;
    int pcount = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            while (*name == ' ') name++;
            int found = 0;
            for (int i = 0; i < pcount; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Project *tmp = realloc(projects, (pcount + 1) * sizeof(Project));
                if (tmp) {
                    projects = tmp;
                    projects[pcount].name = strdup(name);
                    projects[pcount].tasks = NULL;
                    projects[pcount].tcount = 0;
                    pcount++;
                }
            }
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char proj[256], title[256];
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", proj, title) == 2) {
                char *note = strchr(p, ' ');
                if (note) {
                    note = strchr(note + 1, ' ');
                    if (note) {
                        while (*note == ' ') note++;
                    }
                }
                if (!note) note = "";
                for (int i = 0; i < pcount; i++) {
                    if (strcmp(projects[i].name, proj) == 0) {
                        Task *ttmp = realloc(projects[i].tasks, (projects[i].tcount + 1) * sizeof(Task));
                        if (ttmp) {
                            projects[i].tasks = ttmp;
                            projects[i].tasks[projects[i].tcount].title = strdup(title);
                            projects[i].tasks[projects[i].tcount].note = strdup(note);
                            projects[i].tasks[projects[i].tcount].done = 0;
                            projects[i].tcount++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char fromP[256], toP[256], title[256];
            if (sscanf(line + 9, "%255s %255s %255s", fromP, toP, title) == 3) {
                int fidx = -1, tidx = -1;
                for (int i = 0; i < pcount; i++) {
                    if (strcmp(projects[i].name, fromP) == 0) fidx = i;
                    if (strcmp(projects[i].name, toP) == 0) tidx = i;
                }
                if (fidx != -1 && tidx != -1) {
                    for (int j = 0; j < projects[fidx].tcount; j++) {
                        if (strcmp(projects[fidx].tasks[j].title, title) == 0) {
                            Task *ttmp = realloc(projects[tidx].tasks, (projects[tidx].tcount + 1) * sizeof(Task));
                            if (ttmp) {
                                projects[tidx].tasks = ttmp;
                                projects[tidx].tasks[projects[tidx].tcount] = projects[fidx].tasks[j];
                                projects[tidx].tcount++;
                                for (int k = j; k < projects[fidx].tcount - 1; k++) {
                                    projects[fidx].tasks[k] = projects[fidx].tasks[k + 1];
                                }
                                projects[fidx].tcount--;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char proj[256], title[256];
            if (sscanf(line + 9, "%255s %255s", proj, title) == 2) {
                for (int i = 0; i < pcount; i++) {
                    if (strcmp(projects[i].name, proj) == 0) {
                        for (int j = 0; j < projects[i].tcount; j++) {
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
            char *name = line + 14;
            while (*name == ' ') name++;
            for (int i = 0; i < pcount; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (int j = 0; j < projects[i].tcount; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (int j = i; j < pcount - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    pcount--;
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < pcount; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].tcount; j++) {
                    printf("  Task: %s [%s] - %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
    }
    free(line);
    for (int i = 0; i < pcount; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].tcount; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
