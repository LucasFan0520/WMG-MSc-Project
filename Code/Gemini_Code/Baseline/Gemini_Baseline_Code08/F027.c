// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

Reminder *reminders = NULL;
int count = 0;
int capacity = 0;
int next_id = 0;

void add_reminder(int priority, const char *title, const char *note) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        reminders = realloc(reminders, capacity * sizeof(Reminder));
    }
    reminders[count].priority = priority;
    reminders[count].title = strdup(title);
    reminders[count].note = strdup(note);
    reminders[count].id = next_id++;
    count++;
}

void bump_reminder(const char *title, int delta) {
    for (int i = 0; i < count; i++) {
        if (strcmp(reminders[i].title, title) == 0) {
            reminders[i].priority += delta;
            break;
        }
    }
}

void remove_reminder(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(reminders[i].title, title) == 0) {
            free(reminders[i].title);
            free(reminders[i].note);
            for (int j = i; j < count - 1; j++) {
                reminders[j] = reminders[j + 1];
            }
            count--;
            break;
        }
    }
}

int compare_reminders(const void *a, const void *b) {
    const Reminder *rA = (const Reminder *)a;
    const Reminder *rB = (const Reminder *)b;
    if (rB->priority != rA->priority) {
        return rB->priority - rA->priority;
    }
    return rA->id - rB->id;
}

void print_top(int n) {
    if (count == 0) return;
    Reminder *temp = malloc(count * sizeof(Reminder));
    memcpy(temp, reminders, count * sizeof(Reminder));
    qsort(temp, count, sizeof(Reminder), compare_reminders);
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
    }
    free(temp);
}

void list_reminders(void) {
    for (int i = 0; i < count; i++) {
        printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
    }
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
            char *p_s = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (p_s && title && note) {
                add_reminder(atoi(p_s), title, note);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = strtok(NULL, " ");
            char *d_s = strtok(NULL, "");
            if (title && d_s) {
                bump_reminder(title, atoi(d_s));
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, "");
            if (title) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_s = strtok(NULL, "");
            if (n_s) print_top(atoi(n_s));
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}
