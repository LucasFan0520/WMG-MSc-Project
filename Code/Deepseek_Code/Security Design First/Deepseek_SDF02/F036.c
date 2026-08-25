/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *name;
    char *data;
    struct record *next;
} record_t;

record_t *head = NULL;
record_t *tail = NULL;

record_t *find_record(const char *name) {
    record_t *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_record(const char *name, const char *data) {
    if (find_record(name)) return;
    record_t *r = malloc(sizeof(record_t));
    if (!r) return;
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = NULL;
    if (!r->name || !r->data) {
        free(r->name); free(r->data); free(r);
        return;
    }
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void delete_record(const char *name) {
    record_t *prev = NULL, *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
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
    record_t *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = r->data; *s; s++) {
        if (*s == ' ') putchar('_');
        else putchar(*s);
    }
    putchar('\n');
}

void report_names(void) {
    record_t *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all(void) {
    record_t *r = head;
    while (r) {
        record_t *nxt = r->next;
        free(r->name);
        free(r->data);
        free(r);
        r = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256];
        if (sscanf(line, "%9s %255s", cmd, a) >= 1) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                // parse length:data
                char *colon = strchr(rest, ':');
                if (colon) {
                    int len = atoi(rest);
                    if (len > 0 && (size_t)len == strlen(colon + 1)) {
                        add_record(a, colon + 1);
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_record(a);
            } else if (strcmp(cmd, "SHOW") == 0) {
                show_record(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_names();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}