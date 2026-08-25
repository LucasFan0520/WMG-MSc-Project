// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *tag; char *msg; } Record;
Record *records = NULL;
int rec_count = 0, rec_cap = 0;

void add_record(const char *tag, const char *msg) {
    if (rec_count == rec_cap) {
        rec_cap = rec_cap ? rec_cap * 2 : 4;
        records = realloc(records, sizeof(Record) * rec_cap);
    }
    records[rec_count].tag = strdup(tag);
    records[rec_count].msg = strdup(msg);
    rec_count++;
}

char *first_msg_for_tag(const char *tag) {
    for (int i = 0; i < rec_count; i++)
        if (strcmp(records[i].tag, tag) == 0) return records[i].msg;
    return NULL;
}

void concat(const char *tag1, const char *tag2, const char *newtag) {
    char *m1 = first_msg_for_tag(tag1);
    char *m2 = first_msg_for_tag(tag2);
    if (!m1 || !m2) return;
    char *combined = malloc(strlen(m1) + strlen(m2) + 2);
    strcpy(combined, m1);
    strcat(combined, " ");
    strcat(combined, m2);
    add_record(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    int i = 0;
    while (i < rec_count) {
        if (strcmp(records[i].tag, tag) == 0) {
            free(records[i].tag);
            free(records[i].msg);
            for (int j = i; j < rec_count - 1; j++) records[j] = records[j + 1];
            rec_count--;
        } else i++;
    }
}

void find_tag(const char *tag) {
    char *msg = first_msg_for_tag(tag);
    if (msg) {
        for (char *p = msg; *p; p++) putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
    } else printf("NOT_FOUND\n");
}

void report(void) {
    for (int i = 0; i < rec_count; i++)
        printf("%s %s\n", records[i].tag, records[i].msg);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], rest[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            add_record(tag, p);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095s", arg1, arg2, rest) == 3)
                concat(arg1, arg2, rest);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    for (int i = 0; i < rec_count; i++) { free(records[i].tag); free(records[i].msg); }
    free(records);
    return 0;
}