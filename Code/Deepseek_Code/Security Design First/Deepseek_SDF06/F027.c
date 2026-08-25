// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    char *title;
    char *note;
    int priority;
} Reminder;

static Reminder *reminders = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_reminder(const char *title) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(reminders[i].title, title) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Reminder *new = realloc(reminders, capacity * sizeof(Reminder));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        reminders = new;
    }
    reminders[count].title = strdup(title);
    reminders[count].note = strdup(note);
    reminders[count].priority = priority;
    if (!reminders[count].title || !reminders[count].note) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void bump_priority(const char *title, int delta) {
    ssize_t idx = find_reminder(title);
    if (idx == -1) return;
    reminders[idx].priority += delta;
}

static void remove_reminder(const char *title) {
    ssize_t idx = find_reminder(title);
    if (idx == -1) return;
    free(reminders[idx].title);
    free(reminders[idx].note);
    for (size_t i = idx; i < count - 1; i++)
        reminders[i] = reminders[i + 1];
    count--;
}

static int cmp_priority(const void *a, const void *b) {
    const Reminder *ra = (const Reminder *)a;
    const Reminder *rb = (const Reminder *)b;
    return rb->priority - ra->priority; // descending
}

static void top_n(size_t n) {
    if (count == 0) return;
    Reminder *copy = malloc(count * sizeof(Reminder));
    if (!copy) { fprintf(stderr, "Memory error\n"); exit(1); }
    memcpy(copy, reminders, count * sizeof(Reminder));
    qsort(copy, count, sizeof(Reminder), cmp_priority);
    for (size_t i = 0; i < n && i < count; i++)
        printf("%s %s\n", copy[i].title, copy[i].note);
    free(copy);
}

static void list_reminders(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", reminders[i].title, reminders[i].note);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], title[256], note[4096];
    int priority, delta;
    size_t n;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %d %255s %4095[^\n]", &priority, title, note) == 3)
                add_reminder(priority, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &delta) == 2)
                bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(line, "%*s %zu", &n) == 1)
                top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }

    free(line);
    free_all();
    return 0;
}