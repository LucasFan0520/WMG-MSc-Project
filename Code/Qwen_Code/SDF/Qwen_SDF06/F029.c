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

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->key = strdup(key);
    r->value = strdup(value);
    if (!r->key || !r->value) { free(r->key); free(r->value); free(r); return; }
    r->next = head;
    head = r;
}

void dedup() {
    Record *curr = head;
    while (curr) {
        Record *checker = head;
        while (checker) {
            if (checker != curr && strcmp(checker->key, curr->key) == 0) {
                break;
            }
            if (checker == curr) break;
            checker = checker->next;
        }
        if (checker != curr && strcmp(checker->key, curr->key) == 0) {
            Record *next = curr->next;
            Record *prev = NULL;
            Record *c = head;
            while (c != curr) {
                prev = c;
                c = c->next;
            }
            if (prev) prev->next = next;
            else head = next;
            free(curr->key);
            free(curr->value);
            free(curr);
            curr = next;
        } else {
            curr = curr->next;
        }
    }
}

void delete_key(const char *key) {
    Record *curr = head;
    Record *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            Record *next = curr->next;
            if (prev) prev->next = next;
            else head = next;
            free(curr->key);
            free(curr->value);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_key(const char *key) {
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
    }
}

void report() {
    for (Record *r = head; r; r = r->next) {
        printf("%s %s\n", r->key, r->value);
    }
}

void free_all() {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->key);
        free(r->value);
        free(r);
        r = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_record(arg1, arg2);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_key(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_key(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}