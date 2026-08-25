/*
 * F036.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *name;
    char *data;
    struct record *next;
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

void add_record(const char *name, const char *data) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *prev = NULL, *r = records;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->name);
            free(r->data);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = r->data; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report_names(void) {
    Record *r = records;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256];
        if (sscanf(line, "%31s %255s", cmd, name) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(name) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char *colon = strchr(rest, ':');
            if (colon) {
                *colon = 0;
                int len = atoi(rest);
                char *data = colon + 1;
                if (len >= 0 && (int)strlen(data) >= len) {
                    char buf[4096];
                    strncpy(buf, data, len);
                    buf[len] = 0;
                    add_record(name, buf);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    return 0;
}