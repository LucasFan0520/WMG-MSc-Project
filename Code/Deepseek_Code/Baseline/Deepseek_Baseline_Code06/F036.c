/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *head = NULL;

Record* find_record(const char *name) {
    Record *r = head;
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
    r->next = head;
    head = r;
}

void delete_record(const char *name) {
    Record *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
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
    if (!r) return;
    for (char *c = r->data; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report_names() {
    Record *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all() {
    while (head) {
        Record *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], rest[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, name, rest);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            char *colon = strchr(rest, ':');
            if (!colon) continue;
            *colon = '\0';
            int len = atoi(rest);
            char *data = colon + 1;
            if (len < 0) continue;
            if ((int)strlen(data) != len) continue;
            add_record(name, data);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0 && n == 2) {
            show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_names();
        }
    }
    free_all();
    return 0;
}