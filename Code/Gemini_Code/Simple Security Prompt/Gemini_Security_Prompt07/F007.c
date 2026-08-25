// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;
    char *label;
    char *note;
} Activity;

int compare_activities(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    if (actA->time != actB->time) {
        return actA->time - actB->time;
    }
    return strcmp(actA->label, actB->label);
}

int main() {
    Activity *log = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *time_s = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (time_s && label && note) {
                log = realloc(log, (count + 1) * sizeof(Activity));
                log[count].time = atoi(time_s);
                log[count].label = strdup(label);
                log[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time_s = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (time_s && label) {
                int t = atoi(time_s);
                for (int i = 0; i < count; i++) {
                    if (log[i].time == t && strcmp(log[i].label, label) == 0) {
                        free(log[i].label);
                        free(log[i].note);
                        for (int j = i; j < count - 1; j++) {
                            log[j] = log[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(log);
                    log = NULL;
                } else {
                    log = realloc(log, count * sizeof(Activity));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = strtok(NULL, " ");
            if (label) {
                int found_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(log[i].label, label) == 0) {
                        if (found_idx == -1 || log[i].time < log[found_idx].time) {
                            found_idx = i;
                        }
                    }
                }
                if (found_idx != -1) {
                    printf("%d %s %s\n", log[found_idx].time, log[found_idx].label, log[found_idx].note);
                }
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(log, count, sizeof(Activity), compare_activities);
            }
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            free(log);
            log = NULL;
            count = 0;
        }
    }
    for (int i = 0; i < count; i++) {
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
