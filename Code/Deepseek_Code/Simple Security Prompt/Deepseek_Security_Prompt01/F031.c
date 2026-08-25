// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Record;
Record *records = NULL;
int rc = 0, rcap = 0;

int find_record(const char *name) {
    for (int i = 0; i < rc; i++)
        if (strcmp(records[i].name, name) == 0) return i;
    return -1;
}

void add_record(const char *name, const char *note) {
    if (find_record(name) >= 0) return;
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; records = realloc(records, sizeof(Record) * rcap); }
    records[rc].name = strdup(name);
    records[rc].note = strdup(note);
    rc++;
}

void delete_record(const char *name) {
    int idx = find_record(name);
    if (idx < 0) return;
    free(records[idx].name);
    free(records[idx].note);
    for (int i = idx; i < rc - 1; i++) records[i] = records[i + 1];
    rc--;
}

void find_record_print(const char *name) {
    int idx = find_record(name);
    if (idx < 0) return;
    printf("%s\n", records[idx].note);
}

void report_records(void) {
    for (int i = 0; i < rc; i++)
        printf("%s %s\n", records[i].name, records[i].note);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(line, "%*s %d", &n) != 1) continue;
            Record *batch = NULL;
            int bcap = 0, bcount = 0;
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char buf[4096];
                if (!fgets(buf, sizeof(buf), stdin)) { valid = 0; break; }
                buf[strcspn(buf, "\n")] = '\0';
                char name[256], note[4096];
                if (sscanf(buf, "%255s %4095[^\n]", name, note) != 2) { valid = 0; break; }
                if (find_record(name) >= 0) { valid = 0; break; }
                if (bcount == bcap) { bcap = bcap ? bcap * 2 : 4; batch = realloc(batch, sizeof(Record) * bcap); }
                batch[bcount].name = strdup(name);
                batch[bcount].note = strdup(note);
                bcount++;
            }
            if (valid) {
                for (int i = 0; i < bcount; i++) {
                    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; records = realloc(records, sizeof(Record) * rcap); }
                    records[rc] = batch[i];
                    rc++;
                }
            } else {
                for (int i = 0; i < bcount; i++) { free(batch[i].name); free(batch[i].note); }
            }
            free(batch);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) find_record_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) report_records();
    }
    for (int i = 0; i < rc; i++) { free(records[i].name); free(records[i].note); }
    free(records);
    return 0;
}