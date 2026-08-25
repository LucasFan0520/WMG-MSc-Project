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
Record *tail = NULL;

Record *find_record(const char *key, const char *value) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0) return r;
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
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void dedup(void) {
    // Keep only first occurrence for each key
    Record *prev = NULL, *curr = head;
    while (curr) {
        // check if key appears earlier
        Record *check = head;
        int found = 0;
        while (check != curr) {
            if (strcmp(check->key, curr->key) == 0) {
                found = 1;
                break;
            }
            check = check->next;
        }
        if (found) {
            // remove curr
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Record *tmp = curr;
            curr = curr->next;
            free(tmp->key);
            free(tmp->value);
            free(tmp);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void delete_key(const char *key) {
    Record *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->key);
            free(curr->value);
            free(curr);
            curr = prev ? prev->next : head;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_key(const char *key) {
    Record *r = head;
    int first = 1;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (!first) printf(" ");
            printf("%s", r->value);
            first = 0;
        }
        r = r->next;
    }
    printf("\n");
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
        Record *tmp = r;
        r = r->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], value[256];
            if (sscanf(line + 4, "%255s %255s", key, value) == 2) {
                add_record(key, value);
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_key(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_key(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}