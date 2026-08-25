/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *key) {
    Record *r = records;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

Record *find_last_by_key(const char *key) {
    Record *r = records;
    Record *last = NULL;
    while (r) {
        if (strcmp(r->key, key) == 0) last = r;
        r = r->next;
    }
    return last;
}

void free_record(Record *r) {
    free(r->key);
    free(r->value);
    free(r);
}

void add_record(const char *key, const char *value) {
    if (find_record(key)) return; 
    Record *r = malloc(sizeof(Record));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = records;
    records = r;
}

void dedup_records(void) {
    Record *curr = records;
    while (curr) {
        Record *prev = curr;
        Record *next = curr->next;
        while (next) {
            if (strcmp(next->key, curr->key) == 0) {
                prev->next = next->next;
                free_record(next);
                next = prev->next;
            } else {
                prev = next;
                next = next->next;
            }
        }
        curr = curr->next;
    }
}

void delete_key(const char *key) {
    Record *prev = NULL, *curr = records;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            free_record(curr);
            curr = prev ? prev->next : records;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_key_print(const char *key) {
    Record *r = records;
    int found = 0;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->value);
            found = 1;
        }
        r = r->next;
    }
    if (!found) printf("NOT_FOUND\n");
}

void report_records(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->key, r->value);
        r = r->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char key[256], value[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) != 1) continue;
            p += strlen(key);
            while (*p == ' ') p++;
            add_record(key, p);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) == 1) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) == 1) find_key_print(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }

    while (records) {
        Record *tmp = records;
        records = records->next;
        free_record(tmp);
    }
    return 0;
}