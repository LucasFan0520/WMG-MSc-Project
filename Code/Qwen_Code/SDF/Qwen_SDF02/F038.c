// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_COUNT 1000000

typedef struct Encoded {
    char *name;
    char *data;
    struct Encoded *next;
} Encoded;

Encoded *encoded_list = NULL;

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
        
        if (strcmp(cmd, "STORE") == 0) {
            char name[256], data[MAX_LINE];
            if (sscanf(rest, "%255s %4095s", name, data) != 2) continue;
            int valid = 1;
            size_t len = strlen(data);
            size_t i = 0;
            while (i < len) {
                if (!isdigit(data[i])) { valid = 0; break; }
                int count = 0;
                while (i < len && isdigit(data[i])) {
                    count = count * 10 + (data[i] - '0');
                    if (count > MAX_COUNT) { valid = 0; break; }
                    i++;
                }
                if (!valid || i >= len) { valid = 0; break; }
                i++;
            }
            if (!valid) continue;
            Encoded *e = malloc(sizeof(Encoded));
            if (!e) continue;
            e->name = strdup(name);
            e->data = strdup(data);
            if (!e->name || !e->data) { free(e->name); free(e->data); free(e); continue; }
            e->next = encoded_list;
            encoded_list = e;
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            for (Encoded *e = encoded_list; e; e = e->next) {
                if (strcmp(e->name, name) == 0) {
                    size_t len = strlen(e->data);
                    size_t i = 0;
                    while (i < len) {
                        int count = 0;
                        while (i < len && isdigit(e->data[i])) {
                            count = count * 10 + (e->data[i] - '0');
                            i++;
                        }
                        char c = e->data[i++];
                        for (int j = 0; j < count; j++) {
                            putchar(c == ' ' ? '_' : c);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Encoded *prev = NULL;
            for (Encoded *e = encoded_list; e; prev = e, e = e->next) {
                if (strcmp(e->name, name) == 0) {
                    if (prev) prev->next = e->next;
                    else encoded_list = e->next;
                    free(e->name); free(e->data); free(e);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Encoded *e = encoded_list; e; e = e->next) {
                printf("%s\n", e->name);
            }
        }
    }
    while (encoded_list) {
        Encoded *e = encoded_list;
        encoded_list = encoded_list->next;
        free(e->name); free(e->data); free(e);
    }
    return 0;
}