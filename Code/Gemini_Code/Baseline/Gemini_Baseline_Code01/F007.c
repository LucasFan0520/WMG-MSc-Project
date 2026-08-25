// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

int compare_activity(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    int cmp = strcmp(actA->time, actB->time);
    if (cmp != 0) return cmp;
    return strcmp(actA->label, actB->label);
}

int main() {
    Activity *log = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char time_str[256], label_str[256];
            int bytes_parsed = 0;
            if (sscanf(args, "%255s %255s%n", time_str, label_str, &bytes_parsed) == 2) {
                char *note_str = args + bytes_parsed;
                while (*note_str == ' ') note_str++;
                log = realloc(log, (count + 1) * sizeof(Activity));
                log[count].time = strdup(time_str);
                log[count].label = strdup(label_str);
                log[count].note = strdup(note_str);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char time_str[256], label_str[256];
            if (sscanf(args, "%255s %255s", time_str, label_str) == 2) {
                int i = 0;
                while (i < count) {
                    if (strcmp(log[i].time, time_str) == 0 && strcmp(log[i].label, label_str) == 0) {
                        free(log[i].time);
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
                log = realloc(log, count * sizeof(Activity));
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char label_str[256];
            if (sscanf(args, "%255s", label_str) == 1) {
                int earliest_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(log[i].label, label_str) == 0) {
                        if (earliest_idx == -1 || strcmp(log[i].time, log[earliest_idx].time) < 0) {
                            earliest_idx = i;
                        }
                    }
                }
                if (earliest_idx != -1) {
                    printf("%s %s %s\n", log[earliest_idx].time, log[earliest_idx].label, log[earliest_idx].note);
                }
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(log, count, sizeof(Activity), compare_activity);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log[i].time);
                free(log[i].label);
                free(log[i].note);
            }
            free(log);
            log = NULL;
            count = 0;
        }
    }
    for (int i = 0; i < count; i++) {
        free(log[i].time);
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
