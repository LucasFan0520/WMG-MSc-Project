/* F007.c */
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

int main(void) {
    Activity *logs = NULL;
    int count = 0;
    int capacity = 0;
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
            char *space1 = strchr(args, ' ');
            if (!space1) continue;
            *space1 = '\0';
            int t = atoi(args);
            char *rest = space1 + 1;
            char *space2 = strchr(rest, ' ');
            char *lbl = NULL;
            char *nt = NULL;
            if (space2) {
                *space2 = '\0';
                lbl = strdup(rest);
                nt = strdup(space2 + 1);
                *space2 = ' ';
            } else {
                lbl = strdup(rest);
                nt = strdup("");
            }
            *space1 = ' ';
            if (!lbl || !nt) {
                free(lbl);
                free(nt);
                continue;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Activity *tmp = realloc(logs, new_cap * sizeof(Activity));
                if (tmp) {
                    logs = tmp;
                    capacity = new_cap;
                } else {
                    free(lbl);
                    free(nt);
                    continue;
                }
            }
            logs[count].time = t;
            logs[count].label = lbl;
            logs[count].note = nt;
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            int t = atoi(args);
            char *lbl = space + 1;
            int i = 0;
            while (i < count) {
                if (logs[i].time == t && strcmp(logs[i].label, lbl) == 0) {
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
            *space = ' ';
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *lbl = line + 5;
            int best_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(logs[i].label, lbl) == 0) {
                    if (best_idx == -1 || logs[i].time < logs[best_idx].time) {
                        best_idx = i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%d %s %s\n", logs[best_idx].time, logs[best_idx].label, logs[best_idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            if (count > 0) {
                qsort(logs, count, sizeof(Activity), compare_activities);
            }
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", logs[i].time, logs[i].label, logs[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(logs[i].label);
                free(logs[i].note);
            }
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
