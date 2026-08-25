// F036.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} PrefixRecord;

PrefixRecord *records = NULL;
int count = 0;
int capacity = 0;

int find_record(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void add_record(const char *name, const char *len_data) {
    const char *colon = strchr(len_data, ':');
    if (!colon) return;
    int len = atoi(len_data);
    if (len < 0) return;
    if (find_record(name) != -1) return;
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        records = realloc(records, capacity * sizeof(PrefixRecord));
    }
    records[count].name = strdup(name);
    records[count].data = malloc(len + 1);
    strncpy(records[count].data, colon + 1, len);
    records[count].data[len] = '\0';
    count++;
}

void delete_record(const char *name) {
    int idx = find_record(name);
    if (idx != -1) {
        free(records[idx].name);
        free(records[idx].data);
        for (int i = idx; i < count - 1; i++) {
            records[i] = records[i + 1];
        }
        count--;
    }
}

void show_record(const char *name) {
    int idx = find_record(name);
    if (idx != -1) {
        char *d = records[idx].data;
        for (int i = 0; d[i] != '\0'; i++) {
            if (d[i] == ' ') {
                putchar('_');
            } else {
                putchar(d[i]);
            }
        }
        putchar('\n');
    }
}

void report_records(void) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", records[i].name);
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
            char *name = strtok(NULL, " ");
            char *len_data = strtok(NULL, "");
            if (name && len_data) add_record(name, len_data);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(NULL, "");
            if (name) show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}
