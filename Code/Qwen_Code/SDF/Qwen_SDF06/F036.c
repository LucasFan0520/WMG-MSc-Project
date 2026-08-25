// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *head = NULL;

void add_record(const char *name, const char *payload) {
    char *colon = strchr(payload, ':');
    if (!colon) return;
    
    int len = 0;
    if (sscanf(payload, "%d:", &len) != 1) return;
    if (len < 0) return;
    
    char *data_start = colon + 1;
    if ((int)strlen(data_start) < len) return;
    
    char *data = malloc(len + 1);
    if (!data) return;
    strncpy(data, data_start, len);
    data[len] = '\0';
    
    Record *r = malloc(sizeof(Record));
    if (!r) { free(data); return; }
    r->name = strdup(name);
    r->data = data;
    if (!r->name) { free(r->data); free(r); return; }
    r->next = head;
    head = r;
}

void delete_record(const char *name) {
    Record *curr = head;
    Record *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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
    for (Record *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            for (int i = 0; r->data[i]; i++) {
                if (r->data[i] == ' ') putchar('_');
                else putchar(r->data[i]);
            }
            putchar('\n');
            return;
        }
    }
}

void report() {
    for (Record *r = head; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

void free_all() {
    Record *r = head;
    while (r) {
        Record *next = r->next;
        free(r->name);
        free(r->data);
        free(r);
        r = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_record(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_record(arg1);
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            show_record(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}