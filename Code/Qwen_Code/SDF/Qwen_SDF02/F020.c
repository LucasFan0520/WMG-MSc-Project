// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct BodyLine {
    char *text;
    struct BodyLine *next;
} BodyLine;

typedef struct Record {
    char *title;
    BodyLine *body;
    struct Record *next;
} Record;

Record *records = NULL;

Record* find_record(const char *title) {
    for (Record *r = records; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    Record *current = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            current = NULL;
        } else if (!current) {
            current = malloc(sizeof(Record));
            if (!current) continue;
            current->title = strdup(line);
            current->body = NULL;
            current->next = records;
            records = current;
        } else {
            BodyLine *b = malloc(sizeof(BodyLine));
            if (!b) continue;
            b->text = strdup(line);
            b->next = current->body;
            current->body = b;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "FIND") == 0) {
            Record *r = find_record(rest);
            if (r) {
                printf("%s\n", r->title);
                for (BodyLine *b = r->body; b; b = b->next) printf("%s\n", b->text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            Record *prev = NULL;
            for (Record *r = records; r; prev = r, r = r->next) {
                if (strcmp(r->title, rest) == 0) {
                    if (prev) prev->next = r->next;
                    else records = r->next;
                    while (r->body) {
                        BodyLine *b = r->body;
                        r->body = b->next;
                        free(b->text);
                        free(b);
                    }
                    free(r->title);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            if (find_record(rest)) continue;
            Record *r = malloc(sizeof(Record));
            if (!r) continue;
            r->title = strdup(rest);
            r->body = NULL;
            r->next = records;
            records = r;
            
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                BodyLine *b = malloc(sizeof(BodyLine));
                if (!b) continue;
                b->text = strdup(line);
                b->next = r->body;
                r->body = b;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Record *r = records; r; r = r->next) {
                printf("%s\n", r->title);
                for (BodyLine *b = r->body; b; b = b->next) printf("%s\n", b->text);
                printf("--\n");
            }
        }
    }
    while (records) {
        Record *r = records;
        records = records->next;
        while (r->body) {
            BodyLine *b = r->body;
            r->body = b->next;
            free(b->text);
            free(b);
        }
        free(r->title);
        free(r);
    }
    return 0;
}