// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Record {
    char *name;
    char *note;
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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_record(const char *name, const char *note) {
    if (find_record(name)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->name = safe_strdup(name);
    r->note = safe_strdup(note);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *prev = NULL, *cur = records;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else records = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_record_cmd(const char *name) {
    Record *r = find_record(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", r->name, r->note);
}

void report_records(void) {
    Record *r = records;
    while (r) {
        printf("%s %s\n", r->name, r->note);
        r = r->next;
    }
}

void free_all(void) {
    Record *r = records;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->note);
        free(r);
        r = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "BATCH") == 0 && n >= 2) {
            int batch_count = atoi(arg1);
            if (batch_count <= 0) continue;
            // Read batch lines into temporary list
            Record *batch_head = NULL, *batch_tail = NULL;
            int valid = 1;
            for (int i = 0; i < batch_count; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = '\0';
                char name[512], note[MAX_LINE];
                const char *p = line;
                while (*p && !isspace(*p)) p++;
                if (*p == '\0') { valid = 0; break; } // need space
                int len_name = p - line;
                if (len_name >= 512) { valid = 0; break; }
                strncpy(name, line, len_name);
                name[len_name] = '\0';
                while (*p && isspace(*p)) p++;
                strncpy(note, p, sizeof(note)-1);
                note[sizeof(note)-1] = '\0';
                // check if name already exists in main or batch
                if (find_record(name)) { valid = 0; break; }
                Record *cur = batch_head;
                while (cur) {
                    if (strcmp(cur->name, name) == 0) { valid = 0; break; }
                    cur = cur->next;
                }
                if (!valid) break;
                // add to batch list
                Record *r = malloc(sizeof(Record));
                if (!r) { valid = 0; break; }
                r->name = safe_strdup(name);
                r->note = safe_strdup(note);
                r->next = NULL;
                if (batch_tail) batch_tail->next = r;
                else batch_head = r;
                batch_tail = r;
            }
            if (valid) {
                // append batch to main
                if (batch_head) {
                    batch_tail->next = records;
                    records = batch_head;
                }
            } else {
                // free batch
                Record *cur = batch_head;
                while (cur) {
                    Record *next = cur->next;
                    free(cur->name);
                    free(cur->note);
                    free(cur);
                    cur = next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_record_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}