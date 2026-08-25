/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record {
    char *name;
    char *note;
    struct record *next;
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

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    r->name = strdup(name);
    r->note = strdup(note);
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
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void find_record_print(const char *name) {
    Record *r = find_record(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
    for (char *c = r->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void report() {
    Record *r = records;
    while (r) {
        printf("%s ", r->name);
        for (char *c = r->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        r = r->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], note[1024];
        int n;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (strcmp(cmd, "BATCH") == 0) {
            if (sscanf(line, "%*s %d", &n) != 1) continue;
            Record *temp_head = NULL;
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = '\0';
                // parse name and note
                if (sscanf(line, "%255s", name) < 1) { valid = 0; break; }
                char *rest = line + strlen(name) + 1;
                if (*rest == ' ') rest++;
                // store temporarily
                Record *r = malloc(sizeof(Record));
                r->name = strdup(name);
                r->note = strdup(rest);
                r->next = temp_head;
                temp_head = r;
            }
            if (valid) {
                // append all to global list
                Record *r = temp_head;
                while (r) {
                    Record *next = r->next;
                    if (!find_record(r->name)) {
                        r->next = records;
                        records = r;
                    } else {
                        free(r->name);
                        free(r->note);
                        free(r);
                    }
                    r = next;
                }
            } else {
                // discard temp
                Record *r = temp_head;
                while (r) { Record *tmp = r; r = r->next; free(tmp->name); free(tmp->note); free(tmp); }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) find_record_print(name);
        }
    }
    return 0;
}