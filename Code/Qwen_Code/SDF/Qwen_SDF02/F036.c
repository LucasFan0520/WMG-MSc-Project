// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Record {
    char *name;
    char *data;
    struct Record *next;
} Record;

Record *records = NULL;

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", name, &n) != 1) continue;
            char *payload = rest + n;
            while (*payload == ' ') payload++;
            char *colon = strchr(payload, ':');
            if (!colon) continue;
            *colon = '\0';
            int len = atoi(payload);
            if (len < 0) continue;
            char *data = colon + 1;
            if ((int)strlen(data) < len) continue;
            data[len] = '\0';
            Record *r = malloc(sizeof(Record));
            if (!r) continue;
            r->name = strdup(name);
            r->data = strdup(data);
            if (!r->name || !r->data) { free(r->name); free(r->data); free(r); continue; }
            r->next = records;
            records = r;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Record *prev = NULL;
            for (Record *r = records; r; prev = r, r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    if (prev) prev->next = r->next;
                    else records = r->next;
                    free(r->name); free(r->data); free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            for (Record *r = records; r; r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    print_text(r->data);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Record *r = records; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    while (records) {
        Record *r = records;
        records = records->next;
        free(r->name); free(r->data); free(r);
    }
    return 0;
}