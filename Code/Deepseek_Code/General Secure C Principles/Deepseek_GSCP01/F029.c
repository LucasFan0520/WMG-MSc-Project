/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = records;
    records = r;
}

void dedup(void) {
    Record *r = records, *prev = NULL;
    while (r) {
        Record *cur = r;
        Record *runner = r->next;
        while (runner) {
            if (strcmp(runner->key, cur->key) == 0) {
                Record *tofree = runner;
                runner = runner->next;
                if (prev) prev->next = runner;
                else records = runner;
                free(tofree->key);
                free(tofree->value);
                free(tofree);
                continue;
            }
            runner = runner->next;
        }
        prev = r;
        r = r->next;
    }
}

void delete_records(const char *key) {
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

void find_values(const char *key) {
    Record *r = records;
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
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], value[256];
            if (sscanf(line + 4, "%255s %255s", key, value) == 2)
                add_record(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_records(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_values(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}