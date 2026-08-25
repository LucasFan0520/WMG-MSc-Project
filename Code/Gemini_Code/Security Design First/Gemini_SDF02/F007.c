/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long long time;
    char *label;
    char *note;
} Activity;

int compare_activity(const void *a, const void *b) {
    Activity *actA = *(Activity **)a;
    Activity *actB = *(Activity **)b;
    if (actA->time < actB->time) return -1;
    if (actA->time > actB->time) return 1;
    return strcmp(actA->label, actB->label);
}

int main(void) {
    Activity *log = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *time_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note_str = p;

            long long t_val = strtoll(time_str, NULL, 10);
            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Activity *temp = realloc(log, new_cap * sizeof(Activity));
                if (!temp) break;
                log = temp;
                capacity = new_cap;
            }
            log[count].time = t_val;
            log[count].label = strdup(label_str);
            log[count].note = strdup(note_str);
            if (log[count].label && log[count].note) {
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label_str = p;
            long long t_val = strtoll(time_str, NULL, 10);

            for (size_t i = 0; i < count; ) {
                if (log[i].time == t_val && strcmp(log[i].label, label_str) == 0) {
                    free(log[i].label);
                    free(log[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        log[j] = log[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label_str = p;
            long long min_time = -1;
            size_t min_idx = 0;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(log[i].label, label_str) == 0) {
                    if (!found || log[i].time < min_time) {
                        min_time = log[i].time;
                        min_idx = i;
                        found = 1;
                    }
                }
            }
            if (found) {
                printf("%lld %s %s\n", log[min_idx].time, log[min_idx].label, log[min_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                Activity **temp_arr = malloc(count * sizeof(Activity *));
                if (temp_arr) {
                    for (size_t i = 0; i < count; i++) {
                        temp_arr[i] = &log[i];
                    }
                    qsort(temp_arr, count, sizeof(Activity *), compare_activity);
                    for (size_t i = 0; i < count; i++) {
                        printf("%lld %s %s\n", temp_arr[i]->time, temp_arr[i]->label, temp_arr[i]->note);
                    }
                    free(temp_arr);
                }
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            count = 0;
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
