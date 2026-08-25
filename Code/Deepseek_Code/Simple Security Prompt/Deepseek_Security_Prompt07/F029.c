// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *val;
    struct Value *next;
} Value;

typedef struct Record {
    char *key;
    Value *values;
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
    Record *r = find_record(key);
    if (!r) {
        r = malloc(sizeof(Record));
        r->key = strdup(key);
        r->values = NULL;
        r->next = records;
        records = r;
    }
    Value *v = malloc(sizeof(Value));
    v->val = strdup(value);
    v->next = r->values;
    r->values = v;
}

void dedup_records() {
    Record *r = records;
    while (r) {
        // keep only first value for each key (the one added first, but since we prepend, we need to reverse? Actually we need to keep only the first record for each key, which is the earliest added.
        // Since we prepend, the first added is at the end of the list. We'll just keep the last value (the one that was added first).
        // Simpler: we can store in insertion order by appending? But prompt says "keeps only the first record for each key", meaning if multiple values, keep only the first.
        // Since we prepend, the first added is the last in the list. We'll keep only that one.
        if (r->values) {
            Value *first = r->values;
            // find the last value (first inserted)
            Value *last = first;
            while (last->next) last = last->next;
            // free all except last
            Value *cur = first;
            while (cur != last) {
                Value *next = cur->next;
                free(cur->val);
                free(cur);
                cur = next;
            }
            r->values = last;
            last->next = NULL;
        }
        r = r->next;
    }
}

void delete_key(const char *key) {
    Record *prev = NULL;
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            Value *v = r->values;
            while (v) {
                Value *next = v->next;
                free(v->val);
                free(v);
                v = next;
            }
            free(r->key);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void find_key_values(const char *key) {
    Record *r = find_record(key);
    if (!r || !r->values) {
        printf("NOT_FOUND\n");
        return;
    }
    Value *v = r->values;
    int first = 1;
    while (v) {
        if (!first) putchar(' ');
        for (char *s = v->val; *s; s++) putchar(*s == ' ' ? '_' : *s);
        first = 0;
        v = v->next;
    }
    putchar('\n');
}

void report_records() {
    Record *r = records;
    while (r) {
        Value *v = r->values;
        while (v) {
            printf("%s:", r->key);
            for (char *s = v->val; *s; s++) putchar(*s == ' ' ? '_' : *s);
            putchar('\n');
            v = v->next;
        }
        r = r->next;
    }
}

void free_all() {
    Record *r = records;
    while (r) {
        Record *nextr = r->next;
        Value *v = r->values;
        while (v) {
            Value *nextv = v->next;
            free(v->val);
            free(v);
            v = nextv;
        }
        free(r->key);
        free(r);
        r = nextr;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s %[^\n]", key, val) == 2) {
                add_record(key, val);
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_key(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_key_values(rest);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}