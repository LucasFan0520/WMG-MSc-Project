// F036.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
} Record;

static Record *records = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_record(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(records[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_record(const char *name, const char *data) {
    if (find_record(name) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Record *new = realloc(records, capacity * sizeof(Record));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        records = new;
    }
    records[count].name = strdup(name);
    records[count].data = strdup(data);
    if (!records[count].name || !records[count].data) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void delete_record(const char *name) {
    ssize_t idx = find_record(name);
    if (idx == -1) return;
    free(records[idx].name);
    free(records[idx].data);
    for (size_t i = idx; i < count - 1; i++)
        records[i] = records[i + 1];
    count--;
}

static void show_record(const char *name) {
    ssize_t idx = find_record(name);
    if (idx == -1) return;
    for (char *p = records[idx].data; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void report_names(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s\n", records[i].name);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], data_part[8192];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %8191[^\n]", name, data_part) == 2) {
                char *colon = strchr(data_part, ':');
                if (colon) {
                    *colon = '\0';
                    char *len_str = data_part;
                    char *data = colon + 1;
                    unsigned long length = strtoul(len_str, NULL, 10);
                    if (length == strlen(data)) {
                        // valid length
                        add_record(name, data);
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }

    free(line);
    free_all();
    return 0;
}