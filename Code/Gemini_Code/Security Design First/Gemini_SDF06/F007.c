// F007.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

int compare_activities(const void *a, const void *b) {
    const Activity *actA = (const Activity *)a;
    const Activity *actB = (const Activity *)b;
    int cmp = strcmp(actA->time, actB->time);
    if (cmp != 0) return cmp;
    return strcmp(actA->label, actB->label);
}

int main(void) {
    Activity *log_arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *time = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;

            if (*time == '\0' || *label == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Activity *new_arr = realloc(log_arr, new_cap * sizeof(Activity));
                if (!new_arr) continue;
                log_arr = new_arr;
                capacity = new_cap;
            }
            char *t = strdup(time);
            char *l = strdup(label);
            char *n = strdup(note);
            if (t && l && n) {
                log_arr[count].time = t;
                log_arr[count].label = l;
                log_arr[count].note = n;
                count++;
            } else {
                free(t);
                free(l);
                free(n);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *time = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*time == '\0' || *label == '\0') continue;

            size_t i = 0;
            while (i < count) {
                if (strcmp(log_arr[i].time, time) == 0 && strcmp(log_arr[i].label, label) == 0) {
                    free(log_arr[i].time);
                    free(log_arr[i].label);
                    free(log_arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        log_arr[j] = log_arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*label == '\0') continue;

            int best_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(log_arr[i].label, label) == 0) {
                    if (best_idx == -1 || strcmp(log_arr[i].time, log_arr[best_idx].time) < 0) {
                        best_idx = (int)i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%s %s %s\n", log_arr[best_idx].time, log_arr[best_idx].label, log_arr[best_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 1) {
                qsort(log_arr, count, sizeof(Activity), compare_activities);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", log_arr[i].time, log_arr[i].label, log_arr[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(log_arr[i].time);
                free(log_arr[i].label);
                free(log_arr[i].note);
            }
            count = 0;
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(log_arr[i].time);
        free(log_arr[i].label);
        free(log_arr[i].note);
    }
    free(log_arr);
    free(line);
    return 0;
}
