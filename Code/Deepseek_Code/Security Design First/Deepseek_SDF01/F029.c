// filename: F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
} Record;

static Record *records = NULL;

static void cleanup(void) {
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

static Record *find_record(const char *key) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->key, key) == 0)
            return r;
    return NULL;
}

static void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->key = strdup(key);
    r->value = strdup(value);
    if (!r->key || !r->value) {
        free(r->key);
        free(r->value);
        free(r);
        return;
    }
    r->next = records;
    records = r;
}

static void dedup(void) {
    // keep only first occurrence for each key
    Record *new_list = NULL;
    Record *tail = NULL;
    Record *cur = records;
    while (cur) {
        Record *next = cur->next;
        // check if key already in new_list
        int found = 0;
        for (Record *tmp = new_list; tmp; tmp = tmp->next) {
            if (strcmp(tmp->key, cur->key) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            cur->next = NULL;
            if (tail) tail->next = cur;
            else new_list = cur;
            tail = cur;
        } else {
            free(cur->key);
            free(cur->value);
            free(cur);
        }
        cur = next;
    }
    records = new_list;
}

static void delete_key(const char *key) {
    Record *prev = NULL;
    for (Record *r = records; r; r = r->next) {
        if (strcmp(r->key, key) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->key);
            free(r->value);
            free(r);
            r = prev ? prev : records;
            if (!r) break;
        }
        prev = r;
    }
}

static void find_key(const char *key) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->key, key) == 0)
            printf("%s\n", r->value);
}

static void report(void) {
    for (Record *r = records; r; r = r->next)
        printf("%s %s\n", r->key, r->value);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_record(arg1, arg2);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_key(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_key(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free(line);
    return 0;
}