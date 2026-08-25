// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *key) {
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = records;
    records = r;
}

void dedup(void) {
    Record *cur = records;
    while (cur) {
        Record *runner = cur->next;
        Record *prev = cur;
        while (runner) {
            if (strcmp(runner->key, cur->key) == 0) {
                // remove runner
                prev->next = runner->next;
                free(runner->key);
                free(runner->value);
                free(runner);
                runner = prev->next;
            } else {
                prev = runner;
                runner = runner->next;
            }
        }
        cur = cur->next;
    }
}

void delete_key(const char *key) {
    Record *cur = records, *prev = NULL;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            if (prev) cur = prev->next;
            else cur = records;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_key(const char *key) {
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
        r = r->next;
    }
}

void report_records(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

void cleanup_records(void) {
    Record *r = records;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], key[256], value[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, key, value);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    cleanup_records();
    return 0;
}