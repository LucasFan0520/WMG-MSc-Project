// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *records = NULL;

Record *find_record(const char *name) {
    Record *curr = records;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_record(const char *name, const char *length_data) {
    if (find_record(name)) return;
    char *colon = strchr(length_data, ':');
    if (!colon) return;
    
    char len_str[64] = {0};
    size_t len_part = colon - length_data;
    if (len_part >= sizeof(len_str)) return;
    strncpy(len_str, length_data, len_part);
    
    char *endptr;
    long expected_len = strtol(len_str, &endptr, 10);
    if (*endptr != '\0' || expected_len < 0) return;
    
    char *data = colon + 1;
    if ((long)strlen(data) != expected_len) return;
    
    Record *r = malloc(sizeof(Record));
    if (!r) exit(EXIT_FAILURE);
    r->name = strdup(name);
    r->data = strdup(data);
    r->next = records;
    records = r;
}

void delete_record(const char *name) {
    Record *curr = records;
    Record *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            free(curr->name);
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_record(const char *name) {
    Record *r = find_record(name);
    if (!r) return;
    for (int i = 0; r->data[i]; i++) {
        if (r->data[i] == ' ') putchar('_');
        else putchar(r->data[i]);
    }
    putchar('\n');
}

void report_records() {
    Record *curr = records;
    while (curr) {
        printf("%s\n", curr->name);
        curr = curr->next;
    }
}

void free_all() {
    Record *curr = records;
    while (curr) {
        Record *next = curr->next;
        free(curr->name);
        free(curr->data);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %3799[^\n]", cmd, arg1, arg2);
        
        if (strcmp(cmd, "ADD") == 0 && matched == 3) {
            add_record(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record(arg1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_record(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
        }
    }
    free_all();
    return 0;
}