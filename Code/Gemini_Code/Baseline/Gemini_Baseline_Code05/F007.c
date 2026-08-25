// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

Activity *log_items = NULL;
int count = 0;
int capacity = 0;

int compare_activity(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    int cmp = strcmp(actA->time, actB->time);
    if (cmp != 0) {
        return cmp;
    }
    return strcmp(actA->label, actB->label);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                char *p3 = strchr(p2 + 1, ' ');
                if (p3) {
                    *p3 = 0;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        log_items = realloc(log_items, capacity * sizeof(Activity));
                    }
                    log_items[count].time = strdup(p1);
                    log_items[count].label = strdup(p2 + 1);
                    log_items[count].note = strdup(p3 + 1);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int i = 0;
                while (i < count) {
                    if (strcmp(log_items[i].time, p1) == 0 && strcmp(log_items[i].label, p2 + 1) == 0) {
                        free(log_items[i].time);
                        free(log_items[i].label);
                        free(log_items[i].note);
                        for (int j = i; j < count - 1; j++) {
                            log_items[j] = log_items[j + 1];
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
            for (int i = 0; i < count; i++) {
                if (strcmp(log_items[i].label, label) == 0) {
                    if (best_idx == -1 || strcmp(log_items[i].time, log_items[best_idx].time) < 0) {
                        best_idx = i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%s %s %s\n", log_items[best_idx].time, log_items[best_idx].label, log_items[best_idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            qsort(log_items, count, sizeof(Activity), compare_activity);
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", log_items[i].time, log_items[i].label, log_items[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log_items[i].time);
                free(log_items[i].label);
                free(log_items[i].note);
            }
            count = 0;
        }
    }
    for (int i = 0; i < count; i++) {
        free(log_items[i].time);
        free(log_items[i].label);
        free(log_items[i].note);
    }
    free(log_items);
    return 0;
}
