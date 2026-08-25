// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long long priority;
    char *title;
    char *note;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *rA = (const Reminder *)a;
    const Reminder *rB = (const Reminder *)b;
    if (rA->priority != rB->priority) {
        return rA->priority > rB->priority ? -1 : 1;
    }
    return 0;
}

int main(void) {
    Reminder *arr = NULL;
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
            char *prio_str = p;
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                title = p;
            }
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            long long pr = atoll(prio_str);
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Reminder *tmp = realloc(arr, capacity * sizeof(Reminder));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].priority = pr;
            arr[count].title = strdup(title);
            arr[count].note = strdup(note);
            if (!arr[count].title || !arr[count].note) exit(1);
            count++;
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *delta_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                delta_str = p;
            }
            long long delta = atoll(delta_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    arr[i].priority += delta;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            int n = atoi(p);
            if (n > 0) {
                Reminder *cp = malloc(count * sizeof(Reminder));
                if (count > 0 && !cp) exit(1);
                memcpy(cp, arr, count * sizeof(Reminder));
                qsort(cp, count, sizeof(Reminder), compare_reminders);
                size_t limit = (size_t)n < count ? (size_t)n : count;
                for (size_t i = 0; i < limit; i++) {
                    printf("%lld %s: %s\n", cp[i].priority, cp[i].title, cp[i].note);
                }
                free(cp);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s: %s\n", arr[i].priority, arr[i].title, arr[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].note);
    }
    free(arr);
    free(line);
    return 0;
}
