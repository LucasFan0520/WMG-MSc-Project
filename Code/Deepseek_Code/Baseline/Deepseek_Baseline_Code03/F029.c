// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct record {
    char *key;
    char *value;
    struct record *next;
};

struct record *records = NULL;

struct record *find_first_by_key(char *key) {
    struct record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(char *key, char *value) {
    struct record *r = malloc(sizeof(struct record));
    r->key = malloc(strlen(key) + 1);
    strcpy(r->key, key);
    r->value = malloc(strlen(value) + 1);
    strcpy(r->value, value);
    r->next = records;
    records = r;
}

void dedup_records() {
    struct record *r = records, *prev = NULL;
    while (r) {
        struct record *check = records;
        int found = 0;
        while (check != r) {
            if (strcmp(check->key, r->key) == 0) {
                found = 1;
                break;
            }
            check = check->next;
        }
        if (found) {
            // remove r
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->key);
            free(r->value);
            free(r);
            r = prev ? prev->next : records;
        } else {
            prev = r;
            r = r->next;
        }
    }
}

void delete_key(char *key) {
    struct record *r = records, *prev = NULL;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->key);
            free(r->value);
            free(r);
            r = prev ? prev->next : records;
            continue;
        }
        prev = r;
        r = r->next;
    }
}

void find_key(char *key) {
    struct record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
        r = r->next;
    }
}

void report_records() {
    struct record *r = records;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, key, value);
        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    return 0;
}