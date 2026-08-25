// filename: F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

static Record *records = NULL;

static void cleanup(void) {
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

static Record *find_record(const char *name) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->name, name) == 0)
            return r;
    return NULL;
}

static void append_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = strdup(name);
    r->note = strdup(note);
    if (!r->name || !r->note) {
        free(r->name);
        free(r->note);
        free(r);
        return;
    }
    r->next = records;
    records = r;
}

static void delete_record(const char *name) {
    Record *prev = NULL;
    for (Record *r = records; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->name);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
    }
}

static void find_record_print(const char *name) {
    Record *r = find_record(name);
    if (r) printf("%s\n", r->note);
}

static void report_records(void) {
    for (Record *r = records; r; r = r->next)
        printf("%s %s\n", r->name, r->note);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        if (strcmp(cmd, "BATCH") == 0) {
            int n = atoi(arg1);
            if (n <= 0) continue;
            // read n lines, store temporarily
            Record *batch = NULL;
            Record *tail = NULL;
            int valid = 1;
            for (int i = 0; i < n; ++i) {
                char *line2 = NULL;
                size_t len2 = 0;
                if (getline(&line2, &len2, stdin) == -1) {
                    valid = 0;
                    free(line2);
                    break;
                }
                line2[strcspn(line2, "\n")] = '\0';
                char *name = line2;
                char *note = strchr(line2, ' ');
                if (!note) {
                    valid = 0;
                    free(line2);
                    break;
                }
                *note = '\0';
                ++note;
                // check if name already exists in main records? Spec says "if all n lines are valid" - probably valid means non-empty name and note? We'll assume any line with name and note is valid.
                // Also check duplicate names? The spec doesn't say, but we'll allow duplicates? We'll treat as valid.
                Record *r = malloc(sizeof(Record));
                if (!r) { valid = 0; free(line2); break; }
                r->name = strdup(name);
                r->note = strdup(note);
                if (!r->name || !r->note) {
                    free(r->name);
                    free(r->note);
                    free(r);
                    valid = 0;
                    free(line2);
                    break;
                }
                r->next = NULL;
                if (tail) tail->next = r;
                else batch = r;
                tail = r;
                free(line2);
            }
            if (valid) {
                // append batch to records
                if (tail) {
                    tail->next = records;
                    records = batch;
                }
            } else {
                // free batch
                while (batch) {
                    Record *tmp = batch;
                    batch = batch->next;
                    free(tmp->name);
                    free(tmp->note);
                    free(tmp);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_record(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_record_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free(line);
    return 0;
}