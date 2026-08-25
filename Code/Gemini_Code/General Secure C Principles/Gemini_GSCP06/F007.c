// F007.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;
    char *label;
    char *note;
} Activity;

int compare_activities(const void *a, const void *b) {
    const Activity *aa = (const Activity *)a;
    const Activity *bb = (const Activity *)b;
    if (aa->time != bb->time) {
        return aa->time - bb->time;
    }
    return strcmp(aa->label, bb->label);
}

int main(void) {
    Activity *log = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                int t = atoi(args);
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *label = rest;
                    char *note = sp2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Activity *next = realloc(log, capacity * sizeof(Activity));
                        if (!next) break;
                        log = next;
                    }
                    log[count].time = t;
                    log[count].label = strdup(label);
                    log[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                int t = atoi(args);
                char *label = space + 1;
                size_t i = 0;
                while (i < count) {
                    if (log[i].time == t && strcmp(log[i].label, label) == 0) {
                        free(log[i].label);
                        free(log[i].note);
                        for (size_t j = i; j < count - 1; j++) {
                            log[j] = log[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *label = line + 5;
            int best_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(log[i].label, label) == 0) {
                    if (best_idx == -1 || log[i].time < log[best_idx].time) {
                        best_idx = (int)i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%d %s %s\n", log[best_idx].time, log[best_idx].label, log[best_idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            if (count > 0) {
                qsort(log, count, sizeof(Activity), compare_activities);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%d %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            count = 0;
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
