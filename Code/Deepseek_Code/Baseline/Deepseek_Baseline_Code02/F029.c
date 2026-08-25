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

Record *find_record(const char *key, const char *value) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0) return r;
        r = r->next;
    }
    return NULL;
}

int key_exists(const char *key) {
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) return 1;
        r = r->next;
    }
    return 0;
}

void add_record(const char *key, const char *value) {
    if (find_record(key, value)) return;
    Record *r = malloc(sizeof(Record));
    r->key = malloc(strlen(key) + 1);
    strcpy(r->key, key);
    r->value = malloc(strlen(value) + 1);
    strcpy(r->value, value);
    r->next = head;
    head = r;
}

void dedup(void) {
    Record *r = head;
    Record *prev = NULL;
    while (r) {
        Record *next = r->next;
        // check if this key already appeared earlier
        int dup = 0;
        Record *check = head;
        while (check != r) {
            if (strcmp(check->key, r->key) == 0) { dup = 1; break; }
            check = check->next;
        }
        if (dup) {
            // remove r
            if (prev) prev->next = next;
            else head = next;
            free(r->key);
            free(r->value);
            free(r);
        } else {
            prev = r;
        }
        r = next;
    }
}

void delete_key(const char *key) {
    Record *prev = NULL;
    Record *r = head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            Record *next = r->next;
            if (prev) prev->next = next;
            else head = next;
            free(r->key);
            free(r->value);
            free(r);
            r = next;
            continue;
        }
        prev = r;
        r = r->next;
    }
}

void find_key(const char *key) {
    Record *r = head;
    int first = 1;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (!first) putchar(' ');
            printf("%s", r->value);
            first = 0;
        }
        r = r->next;
    }
    putchar('\n');
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_record(key, p);
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