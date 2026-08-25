// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    size_t order;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *rA = (const Reminder *)a;
    const Reminder *rB = (const Reminder *)b;
    if (rB->priority != rA->priority) {
        return (rB->priority > rA->priority) ? 1 : -1;
    }
    return (rA->order > rB->order) ? 1 : -1;
}

int main(void) {
    Reminder *reminders = NULL;
    size_t count = 0;
    size_t capacity = 0;
    size_t total_ordered = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *prio_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;

            if (*prio_str == '\0' || *title == '\0') continue;
            int prio = (int)strtol(prio_str, NULL, 10);

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Reminder *new_rem = realloc(reminders, new_cap * sizeof(Reminder));
                if (!new_rem) continue;
                reminders = new_rem;
                capacity = new_cap;
            }
            char *t = strdup(title);
            char *n = strdup(note);
            if (t && n) {
                reminders[count].priority = prio;
                reminders[count].title = t;
                reminders[count].note = n;
                reminders[count].order = total_ordered++;
                count++;
            } else {
                free(t);
                free(n);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *delta_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*title == '\0' || *delta_str == '\0') continue;
            int delta = (int)strtol(delta_str, NULL, 10);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    reminders[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*title == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    free(reminders[i].title);
                    free(reminders[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        reminders[j] = reminders[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *n_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*n_str == '\0') continue;
            long n_val = strtol(n_str, NULL, 10);
            if (n_val < 0) continue;
            size_t n = (size_t)n_val;

            if (count > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                if (temp) {
                    memcpy(temp, reminders, count * sizeof(Reminder));
                    qsort(temp, count, sizeof(Reminder), compare_reminders);
                    size_t limit = n < count ? n : count;
                    for (size_t i = 0; i < limit; i++) {
                        printf("(%d) %s: %s\n", temp[i].priority, temp[i].title, temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("(%d) %s: %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    free(line);
    return 0;
}
