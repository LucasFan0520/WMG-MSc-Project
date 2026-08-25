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

void add_record(char *key, char *value) {
    Record *r = malloc(sizeof(Record));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = head;
    head = r;
}

void dedup_records() {
    Record *curr = head;
    while (curr) {
        Record *prev = curr;
        Record *next_node = curr->next;
        while (next_node) {
            if (strcmp(next_node->key, curr->key) == 0) {
                Record *to_delete = next_node;
                prev->next = next_node->next;
                next_node = next_node->next;
                free(to_delete->key);
                free(to_delete->value);
                free(to_delete);
            } else {
                prev = next_node;
                next_node = next_node->next;
            }
        }
        curr = curr->next;
    }
}

void delete_key(char *key) {
    Record *prev = NULL;
    Record *curr = head;
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

void find_key(char *key) {
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
    }
}

void report_records() {
    for (Record *r = head; r; r = r->next) {
        printf("%s %s\n", r->key, r->value);
    }
}

int main() {
    char cmd[20], a[100], b[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s", a);
            fgets(b, sizeof(b), stdin);
            while (b[0] == ' ') memmove(b, b + 1, strlen(b));
            b[strcspn(b, "\n")] = 0;
            add_record(a, b);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_key(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", a);
            find_key(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    return 0;
}