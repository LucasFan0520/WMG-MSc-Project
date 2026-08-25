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

Record *find_record(const char *name) {
    Record *r = records;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->note = strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *cur = records, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_record_cmd(const char *name) {
    Record *r = find_record(name);
    if (r) printf("%s\n", r->note);
}

void report_records(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void cleanup_records(void) {
    Record *r = records;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, arg1, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            int nlines = atoi(arg1);
            int valid = 1;
            char **batch_names = malloc(nlines * sizeof(char *));
            char **batch_notes = malloc(nlines * sizeof(char *));
            for (int i = 0; i < nlines; i++) {
                char buf[4096];
                if (!fgets(buf, sizeof(buf), stdin)) { valid = 0; break; }
                buf[strcspn(buf, "\n")] = 0;
                char name[256], note[2048];
                if (sscanf(buf, "%255s %2047[^\n]", name, note) != 2) { valid = 0; break; }
                batch_names[i] = strdup(name);
                batch_notes[i] = strdup(note);
            }
            if (valid) {
                for (int i = 0; i < nlines; i++) {
                    add_record(batch_names[i], batch_notes[i]);
                }
            }
            for (int i = 0; i < nlines; i++) {
                free(batch_names[i]);
                free(batch_notes[i]);
            }
            free(batch_names);
            free(batch_notes);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_record_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    cleanup_records();
    return 0;
}