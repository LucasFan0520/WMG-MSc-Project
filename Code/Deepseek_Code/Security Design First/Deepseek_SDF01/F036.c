// filename: F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

static Record *records = NULL;

static void cleanup(void) {
    while (records) {
        Record *tmp = records;
        records = records->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
}

static Record *find_record(const char *name) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->name, name) == 0)
            return r;
    return NULL;
}

static void add_record(const char *name, const char *length_data) {
    // length_data format: "length:data"
    char *colon = strchr(length_data, ':');
    if (!colon) return;
    *colon = '\0';
    int length = atoi(length_data);
    if (length < 0) { *colon = ':'; return; }
    const char *data = colon + 1;
    if ((int)strlen(data) != length) { *colon = ':'; return; } // data length must match
    // store
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) { *colon = ':'; return; }
    r->name = strdup(name);
    r->data = strdup(data);
    if (!r->name || !r->data) {
        free(r->name);
        free(r->data);
        free(r);
        *colon = ':';
        return;
    }
    r->next = records;
    records = r;
    *colon = ':'; // restore
}

static void delete_record(const char *name) {
    Record *prev = NULL;
    for (Record *r = records; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else records = r->next;
            free(r->name);
            free(r->data);
            free(r);
            return;
        }
        prev = r;
    }
}

static void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) return;
    for (char *p = r->data; *p; ++p)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void report_names(void) {
    for (Record *r = records; r; r = r->next)
        printf("%s\n", r->name);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_record(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_record(arg1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (*arg1) show_record(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free(line);
    return 0;
}