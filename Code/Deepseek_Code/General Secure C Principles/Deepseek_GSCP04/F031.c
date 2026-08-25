/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *note;
} Record;

static Record *records = NULL;
static size_t rcount = 0, rcap = 0;

static void add_record(const char *name, const char *note) {
    if (rcount == rcap) {
        rcap = rcap ? rcap * 2 : 4;
        records = realloc(records, rcap * sizeof(Record));
        if (!records) { perror("realloc"); exit(1); }
    }
    records[rcount].name = strdup(name);
    records[rcount].note = strdup(note);
    rcount++;
}

static void delete_record(const char *name) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(records[i].name, name) == 0) {
            free(records[i].name);
            free(records[i].note);
            for (size_t j = i; j < rcount - 1; j++)
                records[j] = records[j + 1];
            rcount--;
            return;
        }
    }
}

static void find_record(const char *name) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(records[i].name, name) == 0) {
            printf("%s\n", records[i].note);
            return;
        }
    }
}

static void report_records(void) {
    for (size_t i = 0; i < rcount; i++) {
        printf("%s %s\n", records[i].name, records[i].note);
    }
}

static int is_valid_line(const char *line) {
    // a valid line has at least a name (non-empty)
    char *p = strdup(line);
    char *name = strtok(p, " ");
    int ok = (name && strlen(name) > 0);
    free(p);
    return ok;
}

static void batch_import(int n) {
    char **lines = malloc(n * sizeof(char *));
    if (!lines) { perror("malloc"); exit(1); }
    int valid = 1;
    for (int i = 0; i < n; i++) {
        char buf[4096];
        if (!fgets(buf, sizeof(buf), stdin)) {
            valid = 0;
            break;
        }
        buf[strcspn(buf, "\n")] = '\0';
        lines[i] = strdup(buf);
        if (!is_valid_line(lines[i])) valid = 0;
    }
    if (valid) {
        for (int i = 0; i < n; i++) {
            char *name = strtok(lines[i], " ");
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_record(name, note);
        }
    }
    for (int i = 0; i < n; i++) free(lines[i]);
    free(lines);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            char *nstr = strtok(NULL, " ");
            if (!nstr) continue;
            int n = atoi(nstr);
            if (n > 0) batch_import(n);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    for (size_t i = 0; i < rcount; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}