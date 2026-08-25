// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void add_record(const char *name, const char *data) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->data = malloc(strlen(data) + 1);
    strcpy(r->data, data);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *r = records, *prev = NULL;
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
    if (!r) return;
    for (char *c = r->data; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
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

void process_add(const char *cmd) {
    char name[256];
    const char *colon = strchr(cmd, ':');
    if (!colon) return;
    size_t len_before = colon - cmd;
    if (len_before == 0) return;
    char len_str[32];
    strncpy(len_str, cmd, len_before);
    len_str[len_before] = '\0';
    int length = atoi(len_str);
    if (length <= 0) return;
    // name is before the length? Actually format: "name length:data"
    // We need to parse name separately. The command is "ADD name length:data"
    // We already have rest pointing to the whole part after ADD.
    // We'll extract name: the first token before space.
    char *space = strchr(cmd, ' ');
    if (!space) return;
    *space = '\0';
    strcpy(name, cmd);
    *space = ' ';
    const char *data_part = space + 1;
    // data_part should start with length:data
    // already have colon, verify the length matches
    if (strlen(data_part + len_before + 1) != length) {
        // length mismatch: ignore
        return;
    }
    add_record(name, data_part + len_before + 1);
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *tmp = r->next;
        free(r->name);
        free(r->data);
        free(r);
        r = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            process_add(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(rest, "%255s", name) == 1) show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_names();
        }
    }
    free_all();
    return 0;
}