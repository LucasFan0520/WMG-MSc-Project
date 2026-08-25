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
    Record *prev = NULL;
    while (cur) {
        // check if key appears earlier
        int dup = 0;
        Record *check = records;
        while (check != cur) {
            if (strcmp(check->key, cur->key) == 0) {
                dup = 1;
                break;
            }
            check = check->next;
        }
        if (dup) {
            // remove cur
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            cur = prev ? prev->next : records;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void delete_key(const char *key) {
    Record *prev = NULL, *cur = records;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            cur = prev ? prev->next : records;
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

void free_records(void) {
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char key[256], value[256];
        int n = sscanf(line, "%s %s %s", cmd, key, value);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 3) continue;
            add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_records();
    return 0;
}