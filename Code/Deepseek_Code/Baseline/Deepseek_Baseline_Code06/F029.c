/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
} Record;

Record *head = NULL;

Record* find_by_key_value(const char *key, const char *value) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    if (find_by_key_value(key, value)) return;
    Record *r = malloc(sizeof(Record));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = head;
    head = r;
}

void dedup_records() {
    Record *r = head, *prev = NULL;
    while (r) {
        Record *check = head;
        int first = 1;
        while (check != r) {
            if (strcmp(check->key, r->key) == 0) { first = 0; break; }
            check = check->next;
        }
        if (!first) {
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
        if (strcmp(r->key, key) == 0) printf("%s\n", r->value);
        r = r->next;
    }
}

void report_records() {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

void free_all() {
    while (head) {
        Record *tmp = head;
        head = head->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, key, value);
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0 && n == 1) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_records();
        }
    }
    free_all();
    return 0;
}