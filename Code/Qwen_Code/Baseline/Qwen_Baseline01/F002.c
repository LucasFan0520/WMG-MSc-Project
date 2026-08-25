// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

Record *records = NULL;
int count = 0;

int find_tag(const char *tag) {
    for (int i = 0; i < count; i++)
        if (strcmp(records[i].tag, tag) == 0) return i;
    return -1;
}

void add_rec(const char *tag, const char *msg) {
    records = realloc(records, sizeof(Record) * (count + 1));
    records[count].tag = strdup(tag);
    records[count].message = strdup(msg);
    count++;
}

void concat_rec(const char *t1, const char *t2, const char *nt) {
    int i1 = find_tag(t1);
    int i2 = find_tag(t2);
    if (i1 != -1 && i2 != -1) {
        char *new_msg = malloc(strlen(records[i1].message) + strlen(records[i2].message) + 2);
        sprintf(new_msg, "%s %s", records[i1].message, records[i2].message);
        add_rec(nt, new_msg);
        free(new_msg);
    }
}

void remove_rec(const char *tag) {
    int idx = find_tag(tag);
    while (idx != -1) {
        free(records[idx].tag);
        free(records[idx].message);
        for (int i = idx; i < count - 1; i++) records[i] = records[i + 1];
        count--;
        idx = find_tag(tag);
    }
}

void find_rec(const char *tag) {
    int idx = find_tag(tag);
    if (idx != -1) {
        for (int i = 0; records[idx].message[i]; i++)
            putchar(records[idx].message[i] == ' ' ? '_' : records[idx].message[i]);
        putchar('\n');
    }
}

void report_rec() {
    for (int i = 0; i < count; i++) printf("%s %s\n", records[i].tag, records[i].message);
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000], arg2[1000], arg3[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *msg = line + 4 + strlen(arg1);
            if (*msg == ' ') msg++;
            add_rec(arg1, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            sscanf(line, "%*s %s %s %s", arg1, arg2, arg3);
            concat_rec(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", arg1);
            remove_rec(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            find_rec(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_rec();
        }
    }
    for (int i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    return 0;
}