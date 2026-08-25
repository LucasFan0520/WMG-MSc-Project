// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
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

void add_record(const char *key, const char *value) {
    if (find_record(key, value)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = NULL;
    if (!r->key || !r->value) {
        free(r->key); free(r->value); free(r);
        return;
    }
    if (!head) head = r;
    else {
        Record *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
}

void dedup_records(void) {
    Record *r = head;
    while (r) {
        Record *walk = r->next;
        Record *prev = r;
        while (walk) {
            if (strcmp(walk->key, r->key) == 0) {
                // remove walk
                prev->next = walk->next;
                free(walk->key);
                free(walk->value);
                free(walk);
                walk = prev->next;
            } else {
                prev = walk;
                walk = walk->next;
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
        } else {
            prev = r;
            r = r->next;
        }
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

void report_records(void) {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

void free_all(void) {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->key);
        free(r->value);
        free(r);
        r = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], key[256], value[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, value) == 2)
                add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
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
    free(line);
    free_all();
    return 0;
}