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
    if (actA->time != actB->time) {
        return actA->time < actB->time ? -1 : 1;
    }
    return strcmp(actA->label, actB->label);
}

int main(void) {
    Activity *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
            while (*p == ' ') p++;
            char *t_str = p;
            while (*p && *p != ' ') p++;
            char *lbl = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                lbl = p;
            }
            while (*p && *p != ' ') p++;
            char *nt = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                nt = p;
            }
            long long t_val = atoll(t_str);
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Activity *tmp = realloc(arr, capacity * sizeof(Activity));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].time = t_val;
            arr[count].label = strdup(lbl);
            arr[count].note = strdup(nt);
            if (!arr[count].label || !arr[count].note) exit(1);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *t_str = p;
            while (*p && *p != ' ') p++;
            char *lbl = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                lbl = p;
            }
            long long t_val = atoll(t_str);
            for (size_t i = 0; i < count; ) {
                if (arr[i].time == t_val && strcmp(arr[i].label, lbl) == 0) {
                    free(arr[i].label);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *lbl = line + 5;
            while (*lbl == ' ') lbl++;
            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].label, lbl) == 0) {
                    if (found_idx == -1 || arr[i].time < arr[found_idx].time) {
                        found_idx = (int)i;
                    }
                }
            }
            if (found_idx != -1) {
                printf("%lld %s %s\n", arr[found_idx].time, arr[found_idx].label, arr[found_idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            if (count > 0) {
                qsort(arr, count, sizeof(Activity), compare_activities);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s %s\n", arr[i].time, arr[i].label, arr[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(arr[i].label);
                free(arr[i].note);
            }
            count = 0;
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].label);
        free(arr[i].note);
    }
    free(arr);
    free(line);
    return 0;
}
