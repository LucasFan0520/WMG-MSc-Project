// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *data; } Record;
Record *records = NULL;
int rc = 0, rcap = 0;

int find_record(const char *name) {
    for (int i = 0; i < rc; i++)
        if (strcmp(records[i].name, name) == 0) return i;
    return -1;
}

void add_record(const char *name, const char *data) {
    if (find_record(name) >= 0) return;
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; records = realloc(records, sizeof(Record) * rcap); }
    records[rc].name = strdup(name);
    records[rc].data = strdup(data);
    rc++;
}

void delete_record(const char *name) {
    int idx = find_record(name);
    if (idx < 0) return;
    free(records[idx].name);
    free(records[idx].data);
    for (int i = idx; i < rc - 1; i++) records[i] = records[i + 1];
    rc--;
}

void show_record(const char *name) {
    int idx = find_record(name);
    if (idx < 0) return;
    for (char *p = records[idx].data; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

void report_names(void) {
    for (int i = 0; i < rc; i++) printf("%s\n", records[i].name);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], rest[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            strcpy(name, name_start);
            // Now p points to length:data
            char *colon = strchr(p, ':');
            if (!colon) continue;
            int len = atoi(p);
            if (len < 0 || colon + 1 + len > p + strlen(p)) continue;
            char *data = colon + 1;
            if (strlen(data) != (size_t)len) continue;
            add_record(name, data);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_record(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) show_record(name);
        } else if (strcmp(cmd, "REPORT") == 0) report_names();
    }
    for (int i = 0; i < rc; i++) { free(records[i].name); free(records[i].data); }
    free(records);
    return 0;
}