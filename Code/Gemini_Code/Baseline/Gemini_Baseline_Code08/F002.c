// F002.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

Record *records = NULL;
int count = 0;
int capacity = 0;

void add_r(const char *tag, const char *msg) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        records = realloc(records, capacity * sizeof(Record));
    }
    records[count].tag = strdup(tag);
    records[count].message = strdup(msg);
    count++;
}

char *find_first_msg(const char *tag) {
    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].tag, tag) == 0) {
            return records[i].message;
        }
    }
    return NULL;
}

void concat_r(const char *tag1, const char *tag2, const char *newtag) {
    char *m1 = find_first_msg(tag1);
    char *m2 = find_first_msg(tag2);
    if (m1 && m2) {
        size_t len = strlen(m1) + 1 + strlen(m2) + 1;
        char *new_msg = malloc(len);
        sprintf(new_msg, "%s %s", m1, m2);
        add_r(newtag, new_msg);
        free(new_msg);
    }
}

void remove_r(const char *tag) {
    int i = 0;
    while (i < count) {
        if (strcmp(records[i].tag, tag) == 0) {
            free(records[i].tag);
            free(records[i].message);
            for (int j = i; j < count - 1; j++) {
                records[j] = records[j + 1];
            }
            count--;
        } else {
            i++;
        }
    }
}

void find_r(const char *tag) {
    char *msg = find_first_msg(tag);
    if (msg) {
        for (int i = 0; msg[i] != '\0'; i++) {
            if (msg[i] == ' ') {
                putchar('_');
            } else {
                putchar(msg[i]);
            }
        }
        putchar('\n');
    }
}

void report_r(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", records[i].tag, records[i].message);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = strtok(NULL, " ");
            char *msg = strtok(NULL, "");
            if (tag && msg) add_r(tag, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = strtok(NULL, " ");
            char *tag2 = strtok(NULL, " ");
            char *newtag = strtok(NULL, " ");
            if (tag1 && tag2 && newtag) concat_r(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) remove_r(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) find_r(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_r();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    return 0;
}
