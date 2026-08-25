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

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    if (!r) exit(EXIT_FAILURE);
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = records;
    records = r;
}

void dedup_records() {
    Record *curr = records;
    while (curr) {
        Record *runner = curr;
        while (runner->next) {
            if (strcmp(runner->next->key, curr->key) == 0) {
                Record *to_delete = runner->next;
                runner->next = to_delete->next;
                free(to_delete->key);
                free(to_delete->value);
                free(to_delete);
            } else {
                runner = runner->next;
            }
        }
        curr = curr->next;
    }
}

void delete_key(const char *key) {
    Record *curr = records;
    Record *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            Record *to_delete = curr;
            if (prev) prev->next = curr->next;
            else records = curr->next;
            curr = curr->next;
            free(to_delete->key);
            free(to_delete->value);
            free(to_delete);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_key(const char *key) {
    Record *curr = records;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            printf("%s\n", curr->value);
        }
        curr = curr->next;
    }
}

void report_records() {
    Record *curr = records;
    while (curr) {
        printf("%s %s\n", curr->key, curr->value);
        curr = curr->next;
    }
}

void free_all() {
    Record *curr = records;
    while (curr) {
        Record *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, arg1, arg2);
        
        if (strcmp(cmd, "ADD") == 0 && matched == 3) {
            add_record(arg1, arg2);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_key(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_key(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}