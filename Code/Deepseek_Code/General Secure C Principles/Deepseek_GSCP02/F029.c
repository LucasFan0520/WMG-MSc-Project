// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *key;
    char *value;
    struct Record *next;
} Record;

Record *r_head = NULL;

Record *find_record(const char *key, const char *value) {
    Record *r = r_head;
    while (r) {
        if (strcmp(r->key, key) == 0 && strcmp(r->value, value) == 0) return r;
        r = r->next;
    }
    return NULL;
}

Record *find_first_by_key(const char *key) {
    Record *r = r_head;
    while (r) {
        if (strcmp(r->key, key) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *key, const char *value) {
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->key = strdup(key);
    r->value = strdup(value);
    r->next = r_head;
    r_head = r;
}

void dedup(void) {
    Record *r = r_head;
    while (r) {
        Record *prev = r;
        Record *next = r->next;
        while (next) {
            if (strcmp(next->key, r->key) == 0) {
                // remove next
                prev->next = next->next;
                free(next->key);
                free(next->value);
                free(next);
                next = prev->next;
            } else {
                prev = next;
                next = next->next;
            }
        }
        r = r->next;
    }
}

void delete_key(const char *key) {
    Record *prev = NULL;
    Record *r = r_head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            Record *tofree = r;
            if (prev) prev->next = r->next;
            else r_head = r->next;
            r = r->next;
            free(tofree->key);
            free(tofree->value);
            free(tofree);
            continue;
        }
        prev = r;
        r = r->next;
    }
}

void find_key(const char *key) {
    Record *r = r_head;
    while (r) {
        if (strcmp(r->key, key) == 0) {
            for (char *p = r->value; *p; p++) {
                putchar(*p == ' ' ? '_' : *p);
            }
            putchar('\n');
        }
        r = r->next;
    }
}

void report_all(void) {
    Record *r = r_head;
    while (r) {
        printf("%s ", r->key);
        for (char *p = r->value; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        r = r->next;
    }
}

void free_all(void) {
    while (r_head) {
        Record *next = r_head->next;
        free(r_head->key);
        free(r_head->value);
        free(r_head);
        r_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char key[256], value[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", key, value) == 2) {
                add_record(key, value);
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                delete_key(key);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                find_key(key);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    free(line);
    free_all();
    return 0;
}