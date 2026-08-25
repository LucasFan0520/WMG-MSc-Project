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

Record *find_first_record(char *key) {
    for (Record *r = head; r; r = r->next)
        if (strcmp(r->key, key) == 0) return r;
    return NULL;
}

void add_record(char *key, char *value) {
    Record *r = malloc(sizeof(Record));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = head;
    head = r;
}

void dedup(void) {
    Record *prev = NULL;
    Record *cur = head;
    // We'll keep first occurrence, remove later duplicates
    // We can use a simple nested loop to mark duplicates
    // Since list may be large, we can do it by comparing each node with previous ones.
    // Simpler: use a temporary list of seen keys? But we'll do in-place.
    for (Record *i = head; i; i = i->next) {
        Record *j = i->next;
        Record *jprev = i;
        while (j) {
            if (strcmp(j->key, i->key) == 0) {
                // remove j
                jprev->next = j->next;
                free(j->key);
                free(j->value);
                free(j);
                j = jprev->next;
            } else {
                jprev = j;
                j = j->next;
            }
        }
    }
}

void delete_key(char *key) {
    Record *prev = NULL;
    Record *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            Record *next = cur->next;
            if (prev) prev->next = next;
            else head = next;
            free(cur->key);
            free(cur->value);
            free(cur);
            cur = next;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_key(char *key) {
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->key, key) == 0)
            printf("%s\n", r->value);
    }
}

void report_all(void) {
    for (Record *r = head; r; r = r->next)
        printf("%s %s\n", r->key, r->value);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_record(arg1, arg2);
        } else if (strcmp(cmd, "DEDUP") == 0 && n == 1) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_key(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_key(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}