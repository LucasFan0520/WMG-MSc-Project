/* F036.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *data;
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

static void print_underscore(const char *s) {
    for (size_t i = 0; s[i]; i++) {
        putchar(s[i] == ' ' ? '_' : s[i]);
    }
    putchar('\n');
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

        if (strcmp(cmd, "ADD") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", name, &off2) != 1) continue;
            const char *payload = line + offset + off2;
            while (*payload == ' ') payload++;
            char *colon = strchr(payload, ':');
            if (!colon) continue;
            *colon = '\0';
            int expected_len = 0;
            if (sscanf(payload, "%d", &expected_len) != 1 || expected_len < 0) continue;
            const char *data = colon + 1;
            if ((int)strlen(data) != expected_len) continue;
            if (rcount >= rcap) {
                rcap = rcap == 0 ? 8 : rcap * 2;
                records = realloc(records, sizeof(Record) * rcap);
            }
            records[rcount].name = dup_str(name);
            records[rcount].data = dup_str(data);
            rcount++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_record(name);
            if (idx < 0) continue;
            free(records[idx].name);
            free(records[idx].data);
            for (int i = idx; i < rcount - 1; i++) records[i] = records[i + 1];
            rcount--;
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_record(name);
            if (idx >= 0) print_underscore(records[idx].data);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}