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
Record *tail = NULL;

Record *find_first(const char *key) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    r->key = malloc(strlen(key)+1);
    strcpy(r->key, key);
    r->value = malloc(strlen(value)+1);
    strcpy(r->value, value);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void dedup(void) {
    Record *r = head;
    while (r) {
        Record *prev = r;
        Record *scan = r->next;
        while (scan) {
            if (strcmp(scan->key, r->key) == 0) {
                prev->next = scan->next;
                if (scan == tail) tail = prev;
                free(scan->key);
                free(scan->value);
                free(scan);
                scan = prev->next;
            } else {
                prev = scan;
                scan = scan->next;
            }
        }
        r = r->next;
    }
}

void delete_key(const char *key) {
    Record *prev = NULL;
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(rest, " ");
            if (!key) continue;
            char *value = strtok(NULL, "");
            if (!value) value = "";
            add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = strtok(rest, " ");
            if (key) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = strtok(rest, " ");
            if (key) find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}