/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *name;
    char *data;
    struct record *next;
} Record;

Record *head = NULL;

Record *find_record(const char *name) {
    Record *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *spec) {
    // spec format: length:data
    char *colon = strchr(spec, ':');
    if (!colon) return;
    *colon = '\0';
    int len = atoi(spec);
    *colon = ':';
    if (len < 0) return;
    char *data = colon + 1;
    if ((int)strlen(data) < len) return; // not enough data
    // store exactly len bytes (may include spaces)
    char *datacopy = malloc(len + 1);
    strncpy(datacopy, data, len);
    datacopy[len] = '\0';
    Record *r = find_record(name);
    if (r) {
        free(r->data);
        r->data = datacopy;
        return;
    }
    Record *new = malloc(sizeof(Record));
    new->name = strdup(name);
    new->data = datacopy;
    new->next = head;
    head = new;
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
    if (!r) { printf("NOT_FOUND\n"); return; }
    for (char *c = r->data; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void report_names() {
    Record *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], spec[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report_names(); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            add_record(name, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) show_record(name);
        }
    }
    return 0;
}