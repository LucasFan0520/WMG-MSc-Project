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

int compare_activities(const void *a, const void *b) {
    const Activity *actA = (const Activity *)a;
    const Activity *actB = (const Activity *)b;
    if (actA->time < actB->time) return -1;
    if (actA->time > actB->time) return 1;
    return strcmp(actA->label, actB->label);
}

int main() {
    Activity *logs = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                long long t = atoll(args);
                char *label = space2 + 1;
                while (*label == ' ') label++;
                char *space3 = strchr(label, ' ');
                char *note = "";
                if (space3) {
                    *space3 = '\0';
                    note = space3 + 1;
                    while (*note == ' ') note++;
                }
                logs = realloc(logs, (count + 1) * sizeof(Activity));
                if (!logs) return 1;
                logs[count].time = t;
                logs[count].label = strdup(label);
                logs[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                long long t = atoll(args);
                char *label = space2 + 1;
                while (*label == ' ') label++;
                int idx = 0;
                while (idx < count) {
                    if (logs[idx].time == t && strcmp(logs[idx].label, label) == 0) {
                        free(logs[idx].label);
                        free(logs[idx].note);
                        for (int j = idx; j < count - 1; j++) {
                            logs[j] = logs[j + 1];
                        }
                        count--;
                    } else {
                        idx++;
                    }
                }
                logs = realloc(logs, count * sizeof(Activity));
                if (count > 0 && !logs) return 1;
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            long long earliest_time = -1;
            int earliest_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(logs[i].label, args) == 0) {
                    if (earliest_idx == -1 || logs[i].time < earliest_time) {
                        earliest_time = logs[i].time;
                        earliest_idx = i;
                    }
                }
            }
            if (earliest_idx != -1) {
                printf("%lld %s %s\n", logs[earliest_idx].time, logs[earliest_idx].label, logs[earliest_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(logs, count, sizeof(Activity), compare_activities);
            }
            for (int i = 0; i < count; i++) {
                printf("%lld %s %s\n", logs[i].time, logs[i].label, logs[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(logs[i].label);
                free(logs[i].note);
            }
            free(logs);
            logs = NULL;
            count = 0;
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(logs[i].label);
        free(logs[i].note);
    }
    free(logs);
    return 0;
}
