// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Rec {
    char *key;
    char *val;
    struct Rec *next;
} Rec;

Rec *recs = NULL;

void add_rec(const char *key, const char *val) {
    Rec *r = malloc(sizeof(Rec));
    r->key = strdup(key);
    r->val = strdup(val);
    r->next = recs;
    recs = r;
}

void dedup_recs() {
    Rec *curr = recs;
    while (curr) {
        Rec *prev = curr;
        Rec *next_node = curr->next;
        while (next_node) {
            if (strcmp(next_node->key, curr->key) == 0) {
                prev->next = next_node->next;
                free(next_node->key);
                free(next_node->val);
                free(next_node);
                next_node = prev->next;
            } else {
                prev = next_node;
                next_node = next_node->next;
            }
        }
        curr = curr->next;
    }
}

void delete_key(const char *key) {
    Rec *prev = NULL, *curr = recs;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            Rec *next = curr->next;
            if (prev) prev->next = next;
            else recs = next;
            free(curr->key);
            free(curr->val);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_key(const char *key) {
    for (Rec *r = recs; r; r = r->next) {
        if (strcmp(r->key, key) == 0) {
            printf("%s\n", r->val);
        }
    }
}

void report_recs() {
    for (Rec *r = recs; r; r = r->next) {
        printf("%s %s\n", r->key, r->val);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        
        if (strcmp(cmd, "ADD") == 0) add_rec(arg1, arg2);
        else if (strcmp(cmd, "DEDUP") == 0) dedup_recs();
        else if (strcmp(cmd, "DELETE") == 0) delete_key(arg1);
        else if (strcmp(cmd, "FIND") == 0) find_key(arg1);
        else if (strcmp(cmd, "REPORT") == 0) report_recs();
        
        free(line);
    }
    return 0;
}