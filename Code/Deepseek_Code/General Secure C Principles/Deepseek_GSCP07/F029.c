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

Record* find_record(const char *key, const char *value) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_record(Record *r) {
    free(r->key);
    free(r->value);
    free(r);
}

void handle_add(char *cmd) {
    char *key = strtok(NULL, " ");
    if (!key) return;
    char *val = strtok(NULL, "");
    if (!val) val = "";
    if (find_record(key, val)) return;
    Record *new = malloc(sizeof(Record));
    new->key = strdup(key);
    new->value = strdup(val);
    new->next = NULL;
    if (!head) head = new;
    else {
        Record *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_dedup() {
    // Keep only first record for each key
    Record *r = head;
    while (r) {
        Record *runner = r->next;
        Record *prev = r;
        while (runner) {
            if (strcmp(runner->key, r->key) == 0) {
                prev->next = runner->next;
                free_record(runner);
                runner = prev->next;
            } else {
                prev = runner;
                runner = runner->next;
            }
        }
        r = r->next;
    }
}

void handle_delete(char *cmd) {
    char *key = strtok(NULL, " ");
    if (!key) return;
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free_record(r);
            r = prev ? prev->next : head;
        } else {
            prev = r;
            r = r->next;
        }
    }
}

void handle_find(char *cmd) {
    char *key = strtok(NULL, " ");
    if (!key) return;
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
        r = r->next;
    }
}

void handle_report() {
    Record *r = head;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) handle_add(cmd);
        else if (strcmp(cmd, "DEDUP") == 0) handle_dedup();
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "FIND") == 0) handle_find(cmd);
        else if (strcmp(cmd, "REPORT") == 0) handle_report();
    }
    while (head) {
        Record *tmp = head;
        head = head->next;
        free_record(tmp);
    }
    return 0;
}