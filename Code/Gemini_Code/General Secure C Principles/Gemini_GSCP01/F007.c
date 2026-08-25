// F007.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long long time;
    char *label;
    char *note;
} Activity;

int compare_activity(const void *a, const void *b) {
    const Activity *actA = (const Activity *)a;
    const Activity *actB = (const Activity *)b;
    if (actA->time < actB->time) return -1;
    if (actA->time > actB->time) return 1;
    return strcmp(actA->label, actB->label);
}

int main(void) {
    Activity *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *time_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *label = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            Activity *temp = realloc(list, (count + 1) * sizeof(Activity));
            if (temp) {
                list = temp;
                list[count].time = strtoll(time_s, NULL, 10);
                list[count].label = strdup(label);
                list[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *time_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *label = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            long long t_val = strtoll(time_s, NULL, 10);
            size_t i = 0;
            while (i < count) {
                if (list[i].time == t_val && strcmp(list[i].label, label) == 0) {
                    free(list[i].label);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *label = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].label, label) == 0) {
                    if (found_idx == -1 || list[i].time < list[found_idx].time) {
                        found_idx = (int)i;
                    }
                }
            }
            if (found_idx != -1) {
                printf("%lld %s %s\n", list[found_idx].time, list[found_idx].label, list[found_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(list, count, sizeof(Activity), compare_activity);
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s %s\n", list[i].time, list[i].label, list[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(list[i].label);
                free(list[i].note);
            }
            free(list);
            list = NULL;
            count = 0;
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].label);
        free(list[i].note);
    }
    free(list);
    free(line);
    return 0;
}
