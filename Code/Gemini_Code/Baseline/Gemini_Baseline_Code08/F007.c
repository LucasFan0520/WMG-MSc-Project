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

Activity *log_entries = NULL;
int count = 0;
int capacity = 0;

void add_a(int time, const char *label, const char *note) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        log_entries = realloc(log_entries, capacity * sizeof(Activity));
    }
    log_entries[count].time = time;
    log_entries[count].label = strdup(label);
    log_entries[count].note = strdup(note);
    count++;
}

void delete_a(int time, const char *label) {
    int i = 0;
    while (i < count) {
        if (log_entries[i].time == time && strcmp(log_entries[i].label, label) == 0) {
            free(log_entries[i].label);
            free(log_entries[i].note);
            for (int j = i; j < count - 1; j++) {
                log_entries[j] = log_entries[j + 1];
            }
            count--;
        } else {
            i++;
        }
    }
}

void find_a(const char *label) {
    int earliest_idx = -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(log_entries[i].label, label) == 0) {
            if (earliest_idx == -1 || log_entries[i].time < log_entries[earliest_idx].time) {
                earliest_idx = i;
            }
        }
    }
    if (earliest_idx != -1) {
        printf("%d %s %s\n", log_entries[earliest_idx].time, log_entries[earliest_idx].label, log_entries[earliest_idx].note);
    }
}

int compare_activities(const void *a, const void *b) {
    const Activity *actA = (const Activity *)a;
    const Activity *actB = (const Activity *)b;
    if (actA->time != actB->time) {
        return actA->time - actB->time;
    }
    return strcmp(actA->label, actB->label);
}

void sort_a(void) {
    qsort(log_entries, count, sizeof(Activity), compare_activities);
    for (int i = 0; i < count; i++) {
        printf("%d %s %s\n", log_entries[i].time, log_entries[i].label, log_entries[i].note);
    }
}

void clear_a(void) {
    for (int i = 0; i < count; i++) {
        free(log_entries[i].label);
        free(log_entries[i].note);
    }
    count = 0;
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *time_s = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (time_s && label && note) {
                add_a(atoi(time_s), label, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time_s = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (time_s && label) {
                delete_a(atoi(time_s), label);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = strtok(NULL, " ");
            if (label) find_a(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_a();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_a();
        }
    }
    free(line);
    clear_a();
    free(log_entries);
    return 0;
}
