// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
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

static void add_record(const char *name, const char *note) {
    if (find_record(name) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Record *new = realloc(records, capacity * sizeof(Record));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        records = new;
    }
    records[count].name = strdup(name);
    records[count].note = strdup(note);
    if (!records[count].name || !records[count].note) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void delete_record(const char *name) {
    ssize_t idx = find_record(name);
    if (idx == -1) return;
    free(records[idx].name);
    free(records[idx].note);
    for (size_t i = idx; i < count - 1; i++)
        records[i] = records[i + 1];
    count--;
}

static void find_record_print(const char *name) {
    ssize_t idx = find_record(name);
    if (idx == -1) return;
    printf("%s\n", records[idx].note);
}

static void report_records(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", records[i].name, records[i].note);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], note[4096];
    size_t batch_n;
    int batch_valid;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "BATCH") == 0) {
            if (sscanf(line, "%*s %zu", &batch_n) == 1) {
                // Read n lines, store temporarily
                char **batch_names = malloc(batch_n * sizeof(char *));
                char **batch_notes = malloc(batch_n * sizeof(char *));
                if (!batch_names || !batch_notes) { fprintf(stderr, "Memory error\n"); exit(1); }
                batch_valid = 1;
                for (size_t i = 0; i < batch_n; i++) {
                    char *bline = NULL;
                    size_t blen = 0;
                    if (getline(&bline, &blen, stdin) == -1) {
                        batch_valid = 0;
                        free(bline);
                        break;
                    }
                    if (bline[strlen(bline)-1] == '\n') bline[strlen(bline)-1] = '\0';
                    char bname[256], bnote[4096];
                    if (sscanf(bline, "%255s %4095[^\n]", bname, bnote) != 2) {
                        batch_valid = 0;
                        free(bline);
                        break;
                    }
                    batch_names[i] = strdup(bname);
                    batch_notes[i] = strdup(bnote);
                    if (!batch_names[i] || !batch_notes[i]) { fprintf(stderr, "Memory error\n"); exit(1); }
                    free(bline);
                }
                if (batch_valid) {
                    // Check for duplicates or invalid? The spec says if all n lines are valid, append all.
                    // We consider valid as parsing two fields. Also we need to avoid duplicate names? Not specified, but we can allow duplicates as per ADD behavior? 
                    // We'll follow: if any line fails parsing, none added.
                    for (size_t i = 0; i < batch_n; i++) {
                        if (find_record(batch_names[i]) != -1) {
                            // duplicate? The spec says "append all records" if all valid, but duplicates might be allowed. We'll just add as normal.
                        }
                        add_record(batch_names[i], batch_notes[i]);
                    }
                }
                for (size_t i = 0; i < batch_n; i++) {
                    free(batch_names[i]);
                    free(batch_notes[i]);
                }
                free(batch_names);
                free(batch_notes);
                if (!batch_valid) {
                    // ignore batch, but we already freed, so nothing added
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }

    free(line);
    free_all();
    return 0;
}