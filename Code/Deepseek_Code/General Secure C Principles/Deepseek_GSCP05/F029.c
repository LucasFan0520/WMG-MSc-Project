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

Record *find_record_by_key(const char *key) {
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = records;
    records = r;
}

void dedup(void) {
    // Keep only first occurrence of each key
    Record *new_list = NULL;
    Record *r = records;
    while (r) {
        Record *next = r->next;
        // check if key already in new_list
        int found = 0;
        Record *t = new_list;
        while (t) {
            if (strcmp(t->key, r->key) == 0) {
                found = 1;
                break;
            }
            t = t->next;
        }
        if (!found) {
            r->next = new_list;
            new_list = r;
        } else {
            free(r->key);
            free(r->value);
            free(r);
        }
        r = next;
    }
    records = new_list;
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
        printf("%s: %s\n", r->key, r->value);
        r = r->next;
    }
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->key);
        free(r->value);
        free(r);
        r = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            add_record(key, val);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (space) *space = '\0';
            delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (space) *space = '\0';
            find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}