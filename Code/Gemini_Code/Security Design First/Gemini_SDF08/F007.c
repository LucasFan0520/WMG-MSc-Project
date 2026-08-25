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
    const Activity *pa = (const Activity *)a;
    const Activity *pb = (const Activity *)b;
    if (pa->time < pb->time) return -1;
    if (pa->time > pb->time) return 1;
    return strcmp(pa->label, pb->label);
}

int main(void) {
    Activity *logs = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *time_str = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *label = p2;
                    char *note = s2 + 1;
                    char *endptr;
                    long long tval = strtoll(time_str, &endptr, 10);
                    Activity *tmp = realloc(logs, sizeof(Activity) * (count + 1));
                    if (tmp) {
                        logs = tmp;
                        logs[count].time = tval;
                        logs[count].label = strdup(label);
                        logs[count].note = strdup(note);
                        if (logs[count].label && logs[count].note) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *time_str = p;
                char *label = s1 + 1;
                char *endptr;
                long long tval = strtoll(time_str, &endptr, 10);
                int i = 0;
                while (i < count) {
                    if (logs[i].time == tval && strcmp(logs[i].label, label) == 0) {
                        free(logs[i].label);
                        free(logs[i].note);
                        for (int j = i; j < count - 1; j++) {
                            logs[j] = logs[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *label = line + 5;
            int earliest_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(logs[i].label, label) == 0) {
                    if (earliest_idx == -1 || logs[i].time < logs[earliest_idx].time) {
                        earliest_idx = i;
                    }
                }
            }
            if (earliest_idx != -1) {
                printf("%lld %s %s\n", logs[earliest_idx].time, logs[earliest_idx].label, logs[earliest_idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            if (count > 0) {
                qsort(logs, count, sizeof(Activity), compare_activity);
            }
            for (int i = 0; i < count; i++) {
                printf("%lld %s %s\n", logs[i].time, logs[i].label, logs[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(logs[i].label);
                free(logs[i].note);
            }
            free(logs);
            logs = NULL;
            count = 0;
        }
    }
    for (int i = 0; i < count; i++) {
        free(logs[i].label);
        free(logs[i].note);
    }
    free(logs);
    free(line);
    return 0;
}
