/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_record(Record *r) {
    free(r->name);
    free(r->data);
    free(r);
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
    Record *prev = NULL, *curr = records;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            free_record(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = r->data; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_names(void) {
    Record *r = records;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void process_add(const char *line) {
    char *p = (char *)line;
    while (*p == ' ') p++;
    char name[256];
    if (sscanf(p, "%255s", name) != 1) return;
    p += strlen(name);
    while (*p == ' ') p++;
    char *colon = strchr(p, ':');
    if (!colon) return;
    *colon = '\0';
    char *lenstr = p;
    p = colon + 1;
    char *end;
    long len = strtol(lenstr, &end, 10);
    if (*end != '\0' || len < 0) return;
    if ((size_t)len > strlen(p)) return;
    char *data = malloc(len + 1);
    strncpy(data, p, len);
    data[len] = '\0';
    add_record(name, data);
    free(data);
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            process_add(line + 3);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }

    while (records) {
        Record *tmp = records;
        records = records->next;
        free_record(tmp);
    }
    return 0;
}