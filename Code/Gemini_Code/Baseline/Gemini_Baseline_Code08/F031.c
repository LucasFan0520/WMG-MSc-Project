// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

Record *registry = NULL;
int count = 0;
int capacity = 0;

void add_record(const char *name, const char *note) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        registry = realloc(registry, capacity * sizeof(Record));
    }
    registry[count].name = strdup(name);
    registry[count].note = strdup(note);
    count++;
}

void delete_record(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(registry[i].name, name) == 0) {
            free(registry[i].name);
            free(registry[i].note);
            for (int j = i; j < count - 1; j++) {
                registry[j] = registry[j + 1];
            }
            count--;
            i--;
        }
    }
}

void find_record(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(registry[i].name, name) == 0) {
            printf("%s %s\n", registry[i].name, registry[i].note);
            return;
        }
    }
}

void report_records(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", registry[i].name, registry[i].note);
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
        char *line_copy = strdup(line);
        char *cmd = strtok(line_copy, " ");
        if (!cmd) {
            free(line_copy);
            continue;
        }
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_s = strtok(NULL, " ");
            if (n_s) {
                int n = atoi(n_s);
                char **batch_lines = malloc(n * sizeof(char *));
                int all_valid = 1;
                for (int i = 0; i < n; i++) {
                    char *b_line = NULL;
                    size_t b_len = 0;
                    ssize_t b_read = getline(&b_line, &b_len, stdin);
                    if (b_read > 0 && b_line[b_read - 1] == '\n') {
                        b_line[b_read - 1] = '\0';
                    }
                    batch_lines[i] = b_line;
                    char *space = strchr(b_line, ' ');
                    if (!space || space == b_line) {
                        all_valid = 0;
                    }
                }
                if (all_valid) {
                    for (int i = 0; i < n; i++) {
                        char *space = strchr(batch_lines[i], ' ');
                        *space = '\0';
                        add_record(batch_lines[i], space + 1);
                    }
                }
                for (int i = 0; i < n; i++) {
                    free(batch_lines[i]);
                }
                free(batch_lines);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, "");
            if (name) find_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
        free(line_copy);
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(registry[i].name);
        free(registry[i].note);
    }
    free(registry);
    return 0;
}
