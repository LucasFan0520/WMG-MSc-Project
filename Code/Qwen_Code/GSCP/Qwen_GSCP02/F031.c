/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

static Record *records = NULL;
static int rcount = 0;
static int rcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_record(const char *name) {
    for (int i = 0; i < rcount; i++)
        if (strcmp(records[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "BATCH") == 0) {
            int n = 0;
            if (sscanf(line + offset, "%d", &n) != 1 || n < 0) continue;
            Record *batch = malloc(sizeof(Record) * n);
            if (!batch && n > 0) continue;
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                len = strlen(line);
                if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
                char name[256] = {0};
                int off2 = 0;
                if (sscanf(line, "%255s%n", name, &off2) != 1) { valid = 0; break; }
                const char *note = line + off2;
                while (*note == ' ') note++;
                batch[i].name = dup_str(name);
                batch[i].note = dup_str(note);
                if (!batch[i].name || !batch[i].note) { valid = 0; break; }
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    if (rcount >= rcap) {
                        rcap = rcap == 0 ? 8 : rcap * 2;
                        records = realloc(records, sizeof(Record) * rcap);
                    }
                    records[rcount++] = batch[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    free(batch[i].name);
                    free(batch[i].note);
                }
            }
            free(batch);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_record(name);
            if (idx < 0) continue;
            free(records[idx].name);
            free(records[idx].note);
            for (int i = idx; i < rcount - 1; i++) records[i] = records[i + 1];
            rcount--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_record(name);
            if (idx >= 0) printf("%s %s\n", records[idx].name, records[idx].note);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}