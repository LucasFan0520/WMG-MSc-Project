/* F002.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *tag;
    char *message;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_first(const char *tag) {
    Record *r = records;
    while (r) {
        if (strcmp(r->tag, tag) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *tag, const char *msg) {
    Record *r = malloc(sizeof(Record));
    r->tag = strdup(tag);
    r->message = strdup(msg);
    r->next = records;
    records = r;
}

void delete_all(const char *tag) {
    Record *prev = NULL, *cur = records;
    while (cur) {
        if (strcmp(cur->tag, tag) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->tag);
            free(cur->message);
            free(cur);
            cur = prev ? prev->next : records;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void free_records(void) {
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->tag);
        free(tmp->message);
        free(tmp);
    }
}

char *replace_spaces(const char *s) {
    char *r = strdup(s);
    for (char *p = r; *p; p++) if (*p == ' ') *p = '_';
    return r;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char tag1[256], tag2[256], newtag[256];
        char msg[4096];
        int n = sscanf(line, "%s", cmd);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = '\0';
            while (*p == ' ') p++;
            if (*p) add_record(tag, p);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *t1 = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            char *t2 = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            char *nt = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Record *r1 = find_first(t1);
            Record *r2 = find_first(t2);
            if (r1 && r2) {
                char *combined = malloc(strlen(r1->message) + strlen(r2->message) + 2);
                sprintf(combined, "%s %s", r1->message, r2->message);
                add_record(nt, combined);
                free(combined);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            delete_all(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Record *r = find_first(tag);
            if (r) {
                char *s = replace_spaces(r->message);
                printf("%s\n", s);
                free(s);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            Record *r = records;
            while (r) {
                printf("%s %s\n", r->tag, r->message);
                r = r->next;
            }
        }
    }
    free_records();
    return 0;
}