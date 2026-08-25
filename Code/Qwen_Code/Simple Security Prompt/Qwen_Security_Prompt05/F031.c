// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

Record *records = NULL;
int num_records = 0;

int find_record(const char *name) {
    for (int i = 0; i < num_records; i++) {
        if (strcmp(records[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            sscanf(line, "%*s %d", &n);
            Record *batch = malloc(n * sizeof(Record));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) {
                    valid = 0;
                    break;
                }
                line[strcspn(line, "\n")] = 0;
                char name[256];
                if (sscanf(line, "%s", name) != 1) {
                    valid = 0;
                    break;
                }
                char *note = strchr(line, ' ');
                if (note) {
                    note++;
                } else {
                    note = "";
                }
                batch[i].name = strdup(name);
                batch[i].note = strdup(note);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    records = realloc(records, (num_records + 1) * sizeof(Record));
                    records[num_records] = batch[i];
                    num_records++;
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (batch[i].name) free(batch[i].name);
                    if (batch[i].note) free(batch[i].note);
                }
            }
            free(batch);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_record(name);
            if (idx >= 0) {
                free(records[idx].name);
                free(records[idx].note);
                for (int i = idx; i < num_records - 1; i++) {
                    records[i] = records[i + 1];
                }
                num_records--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_record(name);
            if (idx >= 0) {
                printf("%s %s\n", records[idx].name, records[idx].note);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < num_records; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
    }
    
    for (int i = 0; i < num_records; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    
    return 0;
}