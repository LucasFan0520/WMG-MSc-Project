// F029.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
} Record;

static Record *records = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void add_record(const char *key, const char *value) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Record *new = realloc(records, capacity * sizeof(Record));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        records = new;
    }
    records[count].key = strdup(key);
    records[count].value = strdup(value);
    if (!records[count].key || !records[count].value) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void dedup(void) {
    // Keep first occurrence for each key
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        int duplicate = 0;
        for (size_t j = 0; j < write; j++) {
            if (strcmp(records[j].key, records[i].key) == 0) {
                duplicate = 1;
                break;
            }
        }
        if (duplicate) {
            free(records[i].key);
            free(records[i].value);
        } else {
            if (write != i) records[write] = records[i];
            write++;
        }
    }
    count = write;
}

static void delete_key(const char *key) {
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(records[i].key, key) == 0) {
            free(records[i].key);
            free(records[i].value);
        } else {
            if (write != i) records[write] = records[i];
            write++;
        }
    }
    count = write;
}

static void find_key(const char *key) {
    for (size_t i = 0; i < count; i++) {
        if (strcmp(records[i].key, key) == 0)
            printf("%s\n", records[i].value);
    }
}

static void report_records(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", records[i].key, records[i].value);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], key[256], val[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) == 2)
                add_record(key, val);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }

    free(line);
    free_all();
    return 0;
}