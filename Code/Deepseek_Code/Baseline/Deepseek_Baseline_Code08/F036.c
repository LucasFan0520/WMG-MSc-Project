// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *head = NULL;

Record* find_record(const char *name) {
    Record *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_record(const char *name, const char *spec) {
    if (find_record(name)) return;
    // parse length:data
    char *colon = strchr(spec, ':');
    if (!colon) return;
    *colon = '\0';
    char *len_str = spec;
    char *data = colon + 1;
    // check length is valid integer
    for (char *p = len_str; *p; p++) if (!isdigit(*p)) return;
    int len = atoi(len_str);
    if (len < 0) return;
    if ((int)strlen(data) != len) return;
    Record *new = malloc(sizeof(Record));
    new->name = strdup(name);
    new->data = strdup(data);
    new->next = NULL;
    if (!head) head = new;
    else {
        Record *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void delete_record(const char *name) {
    Record *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
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
    if (!r) return;
    char *p = r->data;
    while (*p) {
        if (*p == ' ') putchar('_');
        else putchar(*p);
        p++;
    }
    putchar('\n');
}

void report_names() {
    Record *cur = head;
    while (cur) {
        printf("%s\n", cur->name);
        cur = cur->next;
    }
}

void free_all() {
    Record *cur = head;
    while (cur) {
        Record *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *spec = strtok(NULL, "");
            if (!spec) continue;
            add_record(name, spec);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(NULL, " ");
            if (name) show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free_all();
    return 0;
}