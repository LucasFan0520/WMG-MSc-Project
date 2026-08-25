// F007.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

static Activity *activities = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void add_activity(const char *time, const char *label, const char *note) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Activity *new = realloc(activities, capacity * sizeof(Activity));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        activities = new;
    }
    activities[count].time = strdup(time);
    activities[count].label = strdup(label);
    activities[count].note = strdup(note);
    if (!activities[count].time || !activities[count].label || !activities[count].note)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void delete_activity(const char *time, const char *label) {
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(activities[i].time, time) == 0 && strcmp(activities[i].label, label) == 0) {
            free(activities[i].time);
            free(activities[i].label);
            free(activities[i].note);
        } else {
            if (write != i) activities[write] = activities[i];
            write++;
        }
    }
    count = write;
}

static void find_earliest(const char *label) {
    ssize_t earliest = -1;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(activities[i].label, label) != 0) continue;
        if (earliest == -1 || strcmp(activities[i].time, activities[earliest].time) < 0)
            earliest = i;
    }
    if (earliest == -1) return;
    printf("%s %s %s\n", activities[earliest].time, activities[earliest].label, activities[earliest].note);
}

static int cmp_activity(const void *a, const void *b) {
    const Activity *aa = (const Activity *)a;
    const Activity *bb = (const Activity *)b;
    int c = strcmp(aa->time, bb->time);
    if (c != 0) return c;
    return strcmp(aa->label, bb->label);
}

static void sort_print(void) {
    qsort(activities, count, sizeof(Activity), cmp_activity);
    for (size_t i = 0; i < count; i++)
        printf("%s %s %s\n", activities[i].time, activities[i].label, activities[i].note);
}

static void clear_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(activities[i].time);
        free(activities[i].label);
        free(activities[i].note);
    }
    count = 0;
}

static void free_all(void) {
    clear_all();
    free(activities);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], time[256], label[256], note[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", time, label, note) == 3)
                add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s %255s", time, label) == 2)
                delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", label) == 1)
                find_earliest(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_print();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }

    free(line);
    free_all();
    return 0;
}