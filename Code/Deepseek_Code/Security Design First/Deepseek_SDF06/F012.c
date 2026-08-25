// F012.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

static Person *queue = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_person(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(queue[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void join(const char *name, const char *note) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Person *new = realloc(queue, capacity * sizeof(Person));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        queue = new;
    }
    queue[count].name = strdup(name);
    queue[count].note = strdup(note);
    if (!queue[count].name || !queue[count].note) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void serve(void) {
    if (count == 0) return;
    printf("%s %s\n", queue[0].name, queue[0].note);
    free(queue[0].name);
    free(queue[0].note);
    for (size_t i = 0; i < count - 1; i++)
        queue[i] = queue[i + 1];
    count--;
}

static void cancel(const char *name) {
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(queue[i].name, name) == 0) {
            free(queue[i].name);
            free(queue[i].note);
        } else {
            if (write != i) queue[write] = queue[i];
            write++;
        }
    }
    count = write;
}

static void find_print(const char *name) {
    ssize_t idx = find_person(name);
    if (idx == -1) return;
    printf("%s\n", queue[idx].note);
}

static void queue_print(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", queue[i].name, queue[i].note);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], note[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "JOIN") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2)
                join(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                cancel(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2)
                join(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_print(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_print();
        }
    }

    free(line);
    free_all();
    return 0;
}