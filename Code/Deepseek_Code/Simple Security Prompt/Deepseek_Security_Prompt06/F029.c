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

Record *find_record(const char *key, const char *value) {
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    if (find_record(key, value)) return;
    Record *r = malloc(sizeof(Record));
    r->key = malloc(strlen(key) + 1);
    strcpy(r->key, key);
    r->value = malloc(strlen(value) + 1);
    strcpy(r->value, value);
    r->next = records;
    records = r;
}

void dedup_records(void) {
    Record *r = records, *prev = NULL;
    while (r) {
        Record *check = records;
        int found = 0;
        while (check != r) {
            if (strcmp(check->key, r->key) == 0) {
                found = 1;
                break;
            }
            check = check->next;
        }
        if (found) {
            // remove r
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->key);
            free(r->value);
            free(r);
            r = prev ? prev->next : records;
        } else {
            prev = r;
            r = r->next;
        }
    }
}

void delete_key(const char *key) {
    Record *r = records, *prev = NULL;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->key);
            free(r->value);
            free(r);
            r = prev ? prev->next : records;
        } else {
            prev = r;
            r = r->next;
        }
    }
}

void find_key(const char *key) {
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) printf("%s\n", r->value);
        r = r->next;
    }
}

void report_all(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *tmp = r->next;
        free(r->key);
        free(r->value);
        free(r);
        r = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s", key) == 1) {
                const char *v = rest + strlen(key) + 1;
                while (*v == ' ') v++;
                if (*v) add_record(key, v);
                else add_record(key, "");
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", key) == 1) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", key) == 1) find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}