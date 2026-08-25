/* F002.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

static Record *records = NULL;
static int rcount = 0;
static int rcap = 0;

static int ensure_cap(void) {
    if (rcount >= rcap) {
        int newcap = rcap == 0 ? 8 : rcap * 2;
        Record *tmp = realloc(records, sizeof(Record) * newcap);
        if (!tmp) return -1;
        records = tmp;
        rcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static void print_underscore(const char *s) {
    for (size_t i = 0; s[i]; i++)
        putchar(s[i] == ' ' ? '_' : s[i]);
    putchar('\n');
}

static int find_first(const char *tag) {
    for (int i = 0; i < rcount; i++) {
        if (strcmp(records[i].tag, tag) == 0)
            return i;
    }
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

        if (strcmp(cmd, "ADD") == 0) {
            char tag[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", tag, &off2) != 1) continue;
            const char *msg = line + offset + off2;
            while (*msg == ' ') msg++;
            if (ensure_cap() != 0) continue;
            records[rcount].tag = dup_str(tag);
            records[rcount].message = dup_str(msg);
            if (!records[rcount].tag || !records[rcount].message) {
                free(records[rcount].tag);
                free(records[rcount].message);
                continue;
            }
            rcount++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char tag1[256] = {0}, tag2[256] = {0}, newtag[256] = {0};
            if (sscanf(line + offset, " %255s %255s %255s", tag1, tag2, newtag) != 3) continue;
            int i1 = find_first(tag1);
            int i2 = find_first(tag2);
            if (i1 < 0 || i2 < 0) continue;
            size_t l1 = strlen(records[i1].message);
            size_t l2 = strlen(records[i2].message);
            char *combined = malloc(l1 + 1 + l2 + 1);
            if (!combined) continue;
            memcpy(combined, records[i1].message, l1);
            combined[l1] = ' ';
            memcpy(combined + l1 + 1, records[i2].message, l2 + 1);
            if (ensure_cap() != 0) { free(combined); continue; }
            records[rcount].tag = dup_str(newtag);
            records[rcount].message = combined;
            if (!records[rcount].tag) { free(combined); continue; }
            rcount++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256] = {0};
            if (sscanf(line + offset, " %255s", tag) != 1) continue;
            int i = 0;
            while (i < rcount) {
                if (strcmp(records[i].tag, tag) == 0) {
                    free(records[i].tag);
                    free(records[i].message);
                    for (int j = i; j < rcount - 1; j++)
                        records[j] = records[j + 1];
                    rcount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256] = {0};
            if (sscanf(line + offset, " %255s", tag) != 1) continue;
            int idx = find_first(tag);
            if (idx >= 0) {
                print_underscore(records[idx].message);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s ", records[i].tag);
                print_underscore(records[i].message);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    return 0;
}