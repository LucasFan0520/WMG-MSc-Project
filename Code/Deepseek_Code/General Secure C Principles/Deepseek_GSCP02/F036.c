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

Record *find_record(const char *name) {
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
    if (!r) return;
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = head;
    head = r;
}

void delete_record(const char *name) {
    Record *prev = NULL;
    Record *r = head;
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
    for (char *p = r->data; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
}

void report_names(void) {
    Record *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all(void) {
    while (head) {
        Record *next = head->next;
        free(head->name);
        free(head->data);
        free(head);
        head = next;
    }
}

int is_valid_length(const char *s, int *len) {
    char *end;
    long val = strtol(s, &end, 10);
    if (*end != ':' || val < 0 || val > 1000000) return 0;
    *len = (int)val;
    return 1;
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
            char name[256], rest[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, rest) == 2) {
                int length;
                char *colon = strchr(rest, ':');
                if (colon) {
                    *colon = '\0';
                    if (is_valid_length(rest, &length)) {
                        char *data = colon + 1;
                        if ((int)strlen(data) == length) {
                            add_record(name, data);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_record(name);
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                show_record(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }

    free(line);
    free_all();
    return 0;
}