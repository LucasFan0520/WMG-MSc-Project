/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rec {
    char *key;
    char *value;
    struct rec *next;
} rec_t;

rec_t *head = NULL;
rec_t *tail = NULL;

rec_t *find_record(const char *key, const char *value) {
    rec_t *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    if (find_record(key, value)) return;
    rec_t *r = malloc(sizeof(rec_t));
    if (!r) return;
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = NULL;
    if (!r->key || !r->value) {
        free(r->key); free(r->value); free(r);
        return;
    }
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void dedup(void) {
    // keep only first occurrence per key
    rec_t *prev = NULL, *r = head;
    while (r) {
        // check if key already appeared earlier
        rec_t *check = head;
        int found = 0;
        while (check != r) {
            if (strcmp(check->key, r->key) == 0) { found = 1; break; }
            check = check->next;
        }
        rec_t *nxt = r->next;
        if (found) {
            // remove r
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->key);
            free(r->value);
            free(r);
        } else {
            prev = r;
        }
        r = nxt;
    }
}

void delete_key(const char *key) {
    rec_t *prev = NULL, *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->key);
            free(r->value);
            free(r);
            r = prev ? prev->next : head;
            continue;
        }
        prev = r;
        r = r->next;
    }
}

void find_key(const char *key) {
    rec_t *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
        }
        r = r->next;
    }
}

void report_records(void) {
    rec_t *r = head;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

void free_all(void) {
    rec_t *r = head;
    while (r) {
        rec_t *nxt = r->next;
        free(r->key);
        free(r->value);
        free(r);
        r = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256];
        if (sscanf(line, "%9s %255s", cmd, a) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                const char *val = rest + strlen(a) + 1;
                add_record(a, val);
            } else if (strcmp(cmd, "DEDUP") == 0) {
                dedup();
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_key(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_key(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_records();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}