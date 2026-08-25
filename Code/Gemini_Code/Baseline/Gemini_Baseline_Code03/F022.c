// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int done;
} ProjectTask;

typedef struct {
    char *name;
    ProjectTask *tasks;
    int task_count;
} Project;

int main() {
    Project *projects = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    projects = realloc(projects, (count + 1) * sizeof(Project));
                    projects[count].name = strdup(name);
                    projects[count].tasks = NULL;
                    projects[count].task_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char name[256], title[256];
            int n_len = 0, t_len = 0;
            if (sscanf(ptr, "%255s%n", name, &n_len) > 0) {
                char *ptr2 = ptr + n_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%255s%n", title, &t_len) > 0) {
                    char *note = ptr2 + t_len;
                    while (*note == ' ') note++;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, name) == 0) {
                            int tc = projects[i].task_count;
                            projects[i].tasks = realloc(projects[i].tasks, (tc + 1) * sizeof(ProjectTask));
                            projects[i].tasks[tc].title = strdup(title);
                            projects[i].tasks[tc].note = strdup(note);
                            projects[i].tasks[tc].done = 0;
                            projects[i].task_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            if (sscanf(ptr, "%255s %255s %255s", from, to, title) == 3) {
                int f_idx = -1, t_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, from) == 0) f_idx = i;
                    if (strcmp(projects[i].name, to) == 0) t_idx = i;
                }
                if (f_idx != -1 && t_idx != -1) {
                    int tk_idx = -1;
                    for (int j = 0; j < projects[f_idx].task_count; j++) {
                        if (strcmp(projects[f_idx].tasks[j].title, title) == 0) {
                            tk_idx = j;
                            break;
                        }
                    }
                    if (tk_idx != -1) {
                        int tc = projects[t_idx].task_count;
                        projects[t_idx].tasks = realloc(projects[t_idx].tasks, (tc + 1) * sizeof(ProjectTask));
                        projects[t_idx].tasks[tc] = projects[f_idx].tasks[tk_idx];
                        projects[t_idx].task_count++;
                        for (int j = tk_idx; j < projects[f_idx].task_count - 1; j++) {
                            projects[f_idx].tasks[j] = projects[f_idx].tasks[j + 1];
                        }
                        projects[f_idx].task_count--;
                        if (projects[f_idx].task_count > 0) {
                            projects[f_idx].tasks = realloc(projects[f_idx].tasks, projects[f_idx].task_count * sizeof(ProjectTask));
                        } else {
                            free(projects[f_idx].tasks);
                            projects[f_idx].tasks = NULL;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char name[256], title[256];
            if (sscanf(ptr, "%255s %255s", name, title) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, name) == 0) {
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
            if (sscanf(ptr, "%255s", name) > 0) {
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
                    if (count > 0) {
                        projects = realloc(projects, count * sizeof(Project));
                    } else {
                        free(projects);
                        projects = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("  Task: %s [%s] %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "PENDING", projects[i].tasks[j].note);
                }
            }
        }
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
