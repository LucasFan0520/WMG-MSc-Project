// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    size_t id;
} PriorityReminder;

int compare_reminders(const void *a, const void *b) {
    const PriorityReminder *rA = (const PriorityReminder *)a;
    const PriorityReminder *rB = (const PriorityReminder *)b;
    if (rA->priority > rB->priority) return -1;
    if (rA->priority < rB->priority) return 1;
    if (rA->id < rB->id) return -1;
    return 1;
}

int main(void) {
    PriorityReminder *reminders = NULL;
    size_t count = 0;
    size_t next_id = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *prio_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            PriorityReminder *temp = realloc(reminders, (count + 1) * sizeof(PriorityReminder));
            if (temp) {
                reminders = temp;
                reminders[count].priority = atoi(prio_s);
                reminders[count].title = strdup(title);
                reminders[count].note = strdup(note);
                reminders[count].id = next_id++;
                count++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *delta_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int delta = atoi(delta_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    reminders[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
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
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *n_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int n = atoi(n_s);
            if (n > 0) {
                PriorityReminder *copy = malloc(count * sizeof(PriorityReminder));
                if (copy) {
                    for (size_t i = 0; i < count; i++) {
                        copy[i] = reminders[i];
                    }
                    qsort(copy, count, sizeof(PriorityReminder), compare_reminders);
                    size_t limit = (size_t)n < count ? (size_t)n : count;
                    for (size_t i = 0; i < limit; i++) {
                        printf("(%d) %s: %s\n", copy[i].priority, copy[i].title, copy[i].note);
                    }
                    free(copy);
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
