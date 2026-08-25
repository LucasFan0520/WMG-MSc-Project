/*
 * F029.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *key;
    char *value;
    struct record *next;
} Record;

Record *records = NULL;

Record *find_first(const char *key) {
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

void dedup_records(void) {
    Record *r = records;
    Record *prev = NULL;
    while (r) {
        if (find_first(r->key) != r) {
            // duplicate key, remove this
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->key);
            free(r->value);
            free(r);
            if (prev) r = prev->next;
            else r = records;
        } else {
            prev = r;
            r = r->next;
        }
    }
}

void delete_key(const char *key) {
    Record *prev = NULL, *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->key);
            free(r->value);
            free(r);
            if (prev) r = prev->next;
            else r = records;
        } else {
            prev = r;
            r = r->next;
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], key[256], value[1024];
        if (sscanf(line, "%31s %255s %1023s", cmd, key, value) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(key) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            add_record(key, rest);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    return 0;
}