// F002.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

static Record *records = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void add_record(const char *tag, const char *msg) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Record *new = realloc(records, capacity * sizeof(Record));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        records = new;
    }
    records[count].tag = strdup(tag);
    records[count].message = strdup(msg);
    if (!records[count].tag || !records[count].message) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static ssize_t find_first_by_tag(const char *tag) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(records[i].tag, tag) == 0)
            return (ssize_t)i;
    return -1;
}

static void delete_by_tag(const char *tag) {
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(records[i].tag, tag) != 0) {
            if (write != i) {
                records[write] = records[i];
            }
            write++;
        } else {
            free(records[i].tag);
            free(records[i].message);
        }
    }
    count = write;
}

static void concat(const char *tag1, const char *tag2, const char *newtag) {
    ssize_t idx1 = find_first_by_tag(tag1);
    ssize_t idx2 = find_first_by_tag(tag2);
    if (idx1 == -1 || idx2 == -1) return;
    size_t len = strlen(records[idx1].message) + 1 + strlen(records[idx2].message) + 1;
    char *combined = malloc(len);
    if (!combined) { fprintf(stderr, "Memory error\n"); exit(1); }
    snprintf(combined, len, "%s %s", records[idx1].message, records[idx2].message);
    add_record(newtag, combined);
    free(combined);
}

static void find_print(const char *tag) {
    ssize_t idx = find_first_by_tag(tag);
    if (idx == -1) return;
    for (char *p = records[idx].message; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void report(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", records[i].tag, records[i].message);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], tag1[256], tag2[256], newtag[256], msg[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", tag1, msg) == 2)
                add_record(tag1, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", tag1, tag2, newtag) == 3)
                concat(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", tag1) == 1)
                delete_by_tag(tag1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", tag1) == 1)
                find_print(tag1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }

    free(line);
    free_all();
    return 0;
}