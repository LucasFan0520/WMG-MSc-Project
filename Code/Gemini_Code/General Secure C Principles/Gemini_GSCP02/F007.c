// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long long time;
    char *label;
    char *note;
} Activity;

int compare_activity(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    if (actA->time != actB->time) {
        return (actA->time > actB->time) - (actA->time < actB->time);
    }
    return strcmp(actA->label, actB->label);
}

int main(void) {
    Activity *log = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            long long tm;
            char lbl[256];
            char note[1024];
            if (sscanf(line + 4, "%lld %255s %[^\n]", &tm, lbl, note) == 3) {
                Activity *tmp = realloc(log, (count + 1) * sizeof(Activity));
                if (tmp) {
                    log = tmp;
                    log[count].time = tm;
                    log[count].label = strdup(lbl);
                    log[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            long long tm;
            char lbl[256];
            if (sscanf(line + 7, "%lld %255s", &tm, lbl) == 2) {
                int i = 0;
                while (i < count) {
                    if (log[i].time == tm && strcmp(log[i].label, lbl) == 0) {
                        free(log[i].label);
                        free(log[i].note);
                        for (int j = i; j < count - 1; j++) {
                            log[j] = log[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *lbl = line + 5;
            while (*lbl == ' ') {
                lbl++;
            }
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(log[i].label, lbl) == 0) {
                    if (idx == -1 || log[i].time < log[idx].time) {
                        idx = i;
                    }
                }
            }
            if (idx != -1) {
                printf("%lld %s %s\n", log[idx].time, log[idx].label, log[idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            qsort(log, count, sizeof(Activity), compare_activity);
            for (int i = 0; i < count; i++) {
                printf("%lld %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            free(log);
            log = NULL;
            count = 0;
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
