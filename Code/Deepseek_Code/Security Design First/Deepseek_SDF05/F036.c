// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *name) {
    Record *r = records;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_record(const char *name, const char *data) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = safe_strdup(name);
    r->data = safe_strdup(data);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *prev = NULL, *cur = records;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->name);
            free(cur->data);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    for (const char *s = r->data; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report_names(void) {
    Record *r = records;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->data);
        free(r);
        r = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            // format: length:data
            char *colon = strchr(p, ':');
            if (!colon) continue;
            *colon = '\0';
            int len = atoi(p);
            *colon = ':';
            const char *data = colon + 1;
            int actual_len = strlen(data);
            if (len != actual_len) continue; // invalid
            add_record(arg1, data);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            show_record(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free_all();
    return 0;
}