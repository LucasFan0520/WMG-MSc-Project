// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->key = safe_strdup(key);
    r->value = safe_strdup(value);
    r->next = records;
    records = r;
}

void dedup_records(void) {
    Record *cur = records;
    while (cur) {
        Record *prev = cur, *runner = cur->next;
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
    Record *prev = NULL, *cur = records;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            Record *to_free = cur;
            if (prev) prev->next = cur->next;
            else records = cur->next;
            cur = cur->next;
            free(to_free->key);
            free(to_free->value);
            free(to_free);
            continue;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_key(const char *key) {
    Record *cur = records;
    int found = 0;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            printf("%s\n", cur->value);
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printf("NOT_FOUND\n");
}

void report_records(void) {
    Record *cur = records;
    while (cur) {
        printf("%s %s\n", cur->key, cur->value);
        cur = cur->next;
    }
}

void free_all(void) {
    Record *cur = records;
    while (cur) {
        Record *next = cur->next;
        free(cur->key);
        free(cur->value);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_record(arg1, arg2);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_key(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_key(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}