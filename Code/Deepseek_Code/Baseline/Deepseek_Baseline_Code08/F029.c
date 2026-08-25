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

Record* find_first_record(const char *key) {
    Record *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    Record *new = malloc(sizeof(Record));
    new->key = strdup(key);
    new->value = strdup(value);
    new->next = NULL;
    if (!head) head = new;
    else {
        Record *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void dedup() {
    if (!head) return;
    Record *cur = head;
    Record *prev = NULL;
    Record *new_head = NULL;
    Record *new_tail = NULL;
    Record *cur2 = head;
    while (cur2) {
        int found = 0;
        Record *check = new_head;
        while (check) {
            if (strcmp(check->key, cur2->key) == 0) { found = 1; break; }
            check = check->next;
        }
        if (!found) {
            Record *new_node = malloc(sizeof(Record));
            new_node->key = strdup(cur2->key);
            new_node->value = strdup(cur2->value);
            new_node->next = NULL;
            if (!new_head) new_head = new_node;
            else new_tail->next = new_node;
            new_tail = new_node;
        }
        cur2 = cur2->next;
    }
    // free old list
    Record *old = head;
    while (old) {
        Record *tmp = old;
        old = old->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    head = new_head;
}

void delete_key(const char *key) {
    Record *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            Record *tmp = cur;
            if (prev) prev->next = cur->next;
            else head = cur->next;
            cur = cur->next;
            free(tmp->key);
            free(tmp->value);
            free(tmp);
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_key(const char *key) {
    Record *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            printf("%s\n", cur->value);
        }
        cur = cur->next;
    }
}

void report() {
    Record *cur = head;
    while (cur) {
        printf("%s %s\n", cur->key, cur->value);
        cur = cur->next;
    }
}

void free_all() {
    Record *cur = head;
    while (cur) {
        Record *tmp = cur;
        cur = cur->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            add_record(key, val);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = strtok(NULL, " ");
            if (key) find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}