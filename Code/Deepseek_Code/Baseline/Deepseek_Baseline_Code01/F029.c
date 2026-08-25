// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rec {
    char *key;
    char *value;
    struct rec *next;
} Rec;

Rec *head = NULL;
Rec *tail = NULL;

Rec *find_first_by_key(const char *key) {
    Rec *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    Rec *r = malloc(sizeof(Rec));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void dedup(void) {
    Rec *r = head, *prev = NULL;
    while (r) {
        Rec *scan = head;
        int found = 0;
        while (scan != r) {
            if (strcmp(scan->key, r->key) == 0) { found = 1; break; }
            scan = scan->next;
        }
        if (found) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            Rec *tmp = r;
            r = r->next;
            free(tmp->key);
            free(tmp->value);
            free(tmp);
            continue;
        }
        prev = r;
        r = r->next;
    }
}

void delete_key(const char *key) {
    Rec *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            Rec *tmp = r;
            r = r->next;
            free(tmp->key);
            free(tmp->value);
            free(tmp);
            continue;
        }
        prev = r;
        r = r->next;
    }
}

void find_key(const char *key) {
    Rec *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
        r = r->next;
    }
}

void report_records(void) {
    Rec *r = head;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", key, value) == 2)
                add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    return 0;
}