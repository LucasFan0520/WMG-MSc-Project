// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record { char *key; char *val; struct Record *next; } Record;
Record *records = NULL;

Record *find_record_by_key(const char *key) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->key, key) == 0) return r;
    return NULL;
}

void add_record(const char *key, const char *val) {
    // We do not dedup on ADD; keep all. Dedup later.
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->key = strdup(key);
    r->val = strdup(val);
    r->next = records;
    records = r;
}

void dedup_records(void) {
    Record *newlist = NULL;
    Record *tail = NULL;
    while (records) {
        Record *cur = records;
        records = records->next;
        // check if key already in newlist
        int found = 0;
        for (Record *t = newlist; t; t = t->next) {
            if (strcmp(t->key, cur->key) == 0) { found = 1; break; }
        }
        if (!found) {
            cur->next = NULL;
            if (tail) tail->next = cur; else newlist = cur;
            tail = cur;
        } else {
            free(cur->key); free(cur->val); free(cur);
        }
    }
    records = newlist;
}

void delete_key(const char *key) {
    Record *prev = NULL;
    for (Record *r = records; r; prev = r, r = r->next) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next; else records = r->next;
            free(r->key); free(r->val); free(r);
            r = prev ? prev : records; // continue
        }
    }
}

void find_key(const char *key) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->key, key) == 0)
            printf("%s\n", r->val);
}

void report_records(void) {
    for (Record *r = records; r; r = r->next)
        printf("%s %s\n", r->key, r->val);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s", key) == 1) {
                char *v = rest + strlen(key);
                while (*v == ' ') v++;
                strcpy(val, v);
                add_record(key, val);
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    while (records) { Record *r = records; records = r->next; free(r->key); free(r->val); free(r); }
    return 0;
}