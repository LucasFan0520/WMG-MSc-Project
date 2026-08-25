// F007.c
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

int main(void) {
    Activity *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *time_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *label = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            long long tval = strtoll(time_str, NULL, 10);

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Activity *new_items = realloc(items, capacity * sizeof(Activity));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].time = tval;
            items[count].label = strdup(label);
            items[count].note = strdup(note);
            if (!items[count].label || !items[count].note) exit(1);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long long tval = strtoll(time_str, NULL, 10);

            for (size_t i = 0; i < count; ) {
                if (items[i].time == tval && strcmp(items[i].label, label) == 0) {
                    free(items[i].label);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int best_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].label, label) == 0) {
                    if (best_idx == -1 || items[i].time < items[best_idx].time) {
                        best_idx = (int)i;
                    }
                }
            }

            if (best_idx != -1) {
                printf("%lld %s %s\n", items[best_idx].time, items[best_idx].label, items[best_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(items, count, sizeof(Activity), compare_activities);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s %s\n", items[i].time, items[i].label, items[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(items[i].label);
                free(items[i].note);
            }
            count = 0;
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].label);
        free(items[i].note);
    }
    free(items);
    free(line);
    return 0;
}
