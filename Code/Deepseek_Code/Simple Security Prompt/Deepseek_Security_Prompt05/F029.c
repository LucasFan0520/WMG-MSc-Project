/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *key, const char *value) {
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0)
            return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    if (find_record(key, value)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = records;
    records = r;
}

void dedup(void) {
    Record *outer = records;
    while (outer) {
        Record *inner = outer->next, *prev = outer;
        while (inner) {
            if (strcmp(inner->key, outer->key) == 0) {
                prev->next = inner->next;
                free(inner->key);
                free(inner->value);
                free(inner);
                inner = prev->next;
            } else {
                prev = inner;
                inner = inner->next;
            }
        }
        outer = outer->next;
    }
}

void delete_key(const char *key) {
    Record *r = records, *prev = NULL;
    while (r) {
        if (strcmp(r->key, key) == 0) {
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

void find_key(const char *key) {
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
        r = r->next;
    }
}

void report_all(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_record(arg1, arg2);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_key(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_key(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    return 0;
}