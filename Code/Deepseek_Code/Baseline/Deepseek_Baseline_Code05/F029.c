/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *key;
    char *value;
    struct record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *key, const char *value) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0)
            return r;
        r = r->next;
    }
    return NULL;
}

Record *find_first_by_key(const char *key) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    if (find_record(key, value)) return;
    Record *r = malloc(sizeof(Record));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = head;
    head = r;
}

void dedup() {
    Record *r = head;
    while (r) {
        Record *prev = r;
        Record *s = r->next;
        while (s) {
            if (strcmp(s->key, r->key) == 0) {
                prev->next = s->next;
                free(s->key);
                free(s->value);
                free(s);
                s = prev->next;
            } else {
                prev = s;
                s = s->next;
            }
        }
        r = r->next;
    }
}

void delete_key(const char *key) {
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r->key);
            free(r->value);
            free(r);
            r = prev ? prev->next : head;
            continue;
        }
        prev = r;
        r = r->next;
    }
}

void find_key(const char *key) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
        r = r->next;
    }
}

void report() {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], key[256], val[256];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (strcmp(cmd, "DEDUP") == 0) { dedup(); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s", key, val) == 2)
                add_record(key, val);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) find_key(key);
        }
    }
    return 0;
}