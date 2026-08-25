// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
} Record;

Record *head = NULL, *tail = NULL;

Record *find_record_by_key(const char *key) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    r->key = malloc(strlen(key) + 1);
    strcpy(r->key, key);
    r->value = malloc(strlen(value) + 1);
    strcpy(r->value, value);
    r->next = NULL;
    if (tail) {
        tail->next = r;
        tail = r;
    } else {
        head = tail = r;
    }
}

void dedup_records(void) {
    Record *r = head;
    while (r) {
        Record *prev = r, *curr = r->next;
        while (curr) {
            if (strcmp(curr->key, r->key) == 0) {
                prev->next = curr->next;
                if (curr == tail) tail = prev;
                Record *next = curr->next;
                free(curr->key);
                free(curr->value);
                free(curr);
                curr = next;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
        r = r->next;
    }
}

void delete_key(const char *key) {
    Record *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Record *next = curr->next;
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

void find_key_values(const char *key) {
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, " ");
            if (key && value) add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = strtok(NULL, " ");
            if (key) find_key_values(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free(line);
    free_all();
    return 0;
}