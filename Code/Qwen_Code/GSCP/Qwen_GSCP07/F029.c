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

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = records;
    records = r;
}

void dedup() {
    Record **rp = &records;
    while (*rp) {
        int found = 0;
        for (Record *prev = records; prev != *rp; prev = prev->next) {
            if (strcmp(prev->key, (*rp)->key) == 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            Record *tmp = *rp;
            *rp = tmp->next;
            free(tmp->key);
            free(tmp->value);
            free(tmp);
        } else {
            rp = &(*rp)->next;
        }
    }
}

void delete_key(const char *key) {
    Record **rp = &records;
    while (*rp) {
        if (strcmp((*rp)->key, key) == 0) {
            Record *tmp = *rp;
            *rp = tmp->next;
            free(tmp->key);
            free(tmp->value);
            free(tmp);
        } else {
            rp = &(*rp)->next;
        }
    }
}

void find_key(const char *key) {
    for (Record *r = records; r; r = r->next) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
    }
}

void report() {
    for (Record *r = records; r; r = r->next) {
        printf("%s %s\n", r->key, r->value);
    }
}

void cleanup() {
    while (records) {
        Record *tmp = records->next;
        free(records->key);
        free(records->value);
        free(records);
        records = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) add_record(a1, t + 1); }
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_key(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) find_key(a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    cleanup();
    return 0;
}