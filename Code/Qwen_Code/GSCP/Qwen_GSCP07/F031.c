// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
    struct Record *next;
} Record;

Record *records = NULL;

void add_record(const char *name, const char *note) {
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record **rp = &records;
    while (*rp) {
        if (strcmp((*rp)->name, name) == 0) {
            Record *tmp = *rp;
            *rp = tmp->next;
            free(tmp->name);
            free(tmp->note);
            free(tmp);
            return;
        }
        rp = &(*rp)->next;
    }
}

void find_record(const char *name) {
    for (Record *r = records; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            printf("%s %s\n", r->name, r->note);
            return;
        }
    }
}

void report() {
    for (Record *r = records; r; r = r->next) {
        printf("%s %s\n", r->name, r->note);
    }
}

void cleanup() {
    while (records) {
        Record *tmp = records->next;
        free(records->name);
        free(records->note);
        free(records);
        records = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        int n;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            if (sscanf(line, "%*s %d", &n) == 1) {
                Record *batch = NULL;
                int valid = 1;
                for (int i = 0; i < n; i++) {
                    if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                    line[strcspn(line, "\n")] = 0;
                    char name[256];
                    if (sscanf(line, "%255s", name) != 1) { valid = 0; break; }
                    char *t = strchr(line, ' ');
                    if (!t) { valid = 0; break; }
                    Record *r = malloc(sizeof(Record));
                    r->name = strdup(name);
                    r->note = strdup(t + 1);
                    r->next = batch;
                    batch = r;
                }
                if (valid) {
                    while (batch) {
                        Record *tmp = batch->next;
                        batch->next = records;
                        records = batch;
                        batch = tmp;
                    }
                } else {
                    while (batch) {
                        Record *tmp = batch->next;
                        free(batch->name);
                        free(batch->note);
                        free(batch);
                        batch = tmp;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char a1[256];
            if (sscanf(line, "%*s %255s", a1) == 1) delete_record(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            char a1[256];
            if (sscanf(line, "%*s %255s", a1) == 1) find_record(a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    cleanup();
    return 0;
}