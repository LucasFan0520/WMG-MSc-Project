/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
} Record;

static Record *records = NULL;
static size_t rcount = 0, rcap = 0;

static Record *find_record(const char *name) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(records[i].name, name) == 0)
            return &records[i];
    }
    return NULL;
}

static void add_record(const char *name, const char *data) {
    if (rcount == rcap) {
        rcap = rcap ? rcap * 2 : 4;
        records = realloc(records, rcap * sizeof(Record));
        if (!records) { perror("realloc"); exit(1); }
    }
    records[rcount].name = strdup(name);
    records[rcount].data = strdup(data);
    rcount++;
}

static void delete_record(const char *name) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(records[i].name, name) == 0) {
            free(records[i].name);
            free(records[i].data);
            for (size_t j = i; j < rcount - 1; j++)
                records[j] = records[j + 1];
            rcount--;
            return;
        }
    }
}

static void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) return;
    for (char *c = r->data; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

static void report_names(void) {
    for (size_t i = 0; i < rcount; i++) {
        printf("%s\n", records[i].name);
    }
}

static int parse_length_data(const char *input, char **data_out) {
    char *colon = strchr(input, ':');
    if (!colon) return 0;
    *colon = '\0';
    int len = atoi(input);
    *colon = ':';
    const char *data = colon + 1;
    if ((int)strlen(data) != len) return 0;
    *data_out = strdup(data);
    return 1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *spec = strtok(NULL, "");
            if (!spec) continue;
            char *data = NULL;
            if (parse_length_data(spec, &data)) {
                if (find_record(name)) {
                    Record *r = find_record(name);
                    free(r->data);
                    r->data = data;
                } else {
                    add_record(name, data);
                    free(data);
                }
            }
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
    for (size_t i = 0; i < rcount; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}