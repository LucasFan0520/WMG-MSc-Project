// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_KEY 64
#define MAX_VAL 4096

typedef struct Record {
    char key[MAX_KEY];
    char val[MAX_VAL];
    struct Record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *key, const char *val) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->val, val) == 0) return r;
        r = r->next;
    }
    return NULL;
}

int record_exists(const char *key) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) return 1;
        r = r->next;
    }
    return 0;
}

void free_all() {
    Record *r = head;
    while (r) {
        Record *tmp = r;
        r = r->next;
        free(tmp);
    }
    head = NULL;
}

void handle_add(char *line) {
    char key[MAX_KEY], val[MAX_VAL];
    int n = sscanf(line, "ADD %63s %4095[^\n]", key, val);
    if (n < 2) return;
    if (find_record(key, val)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    strcpy(r->key, key);
    strcpy(r->val, val);
    r->next = head;
    head = r;
}

void handle_dedup() {
    // Keep only first record for each key
    Record *r = head;
    Record *prev = NULL;
    while (r) {
        // Check if there is an earlier occurrence of same key
        Record *check = head;
        int first = 1;
        while (check != r) {
            if (strcmp(check->key, r->key) == 0) {
                first = 0;
                break;
            }
            check = check->next;
        }
        if (!first) {
            // remove r
            Record *to_del = r;
            if (prev) prev->next = r->next;
            else head = r->next;
            r = r->next;
            free(to_del);
        } else {
            prev = r;
            r = r->next;
        }
    }
}

void handle_delete(char *line) {
    char key[MAX_KEY];
    if (sscanf(line, "DELETE %63s", key) != 1) return;
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r);
            r = prev ? prev->next : head;
        } else {
            prev = r;
            r = r->next;
        }
    }
}

void handle_find(char *line) {
    char key[MAX_KEY];
    if (sscanf(line, "FIND %63s", key) != 1) return;
    Record *r = head;
    int first = 1;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (!first) putchar(' ');
            printf("%s", r->val);
            first = 0;
        }
        r = r->next;
    }
    putchar('\n');
}

void handle_report() {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->key, r->val);
        r = r->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strcmp(line, "DEDUP") == 0) handle_dedup();
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}