// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct encoded {
    char *name;
    char *data; // stored as original encoded string
    struct encoded *next;
};

struct encoded *encodings = NULL;

struct encoded *find_encoded(char *name) {
    struct encoded *e = encodings;
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void store_encoded(char *name, char *encoded) {
    if (find_encoded(name)) return;
    struct encoded *e = malloc(sizeof(struct encoded));
    e->name = malloc(strlen(name) + 1);
    strcpy(e->name, name);
    e->data = malloc(strlen(encoded) + 1);
    strcpy(e->data, encoded);
    e->next = encodings;
    encodings = e;
}

void decode_print(char *name) {
    struct encoded *e = find_encoded(name);
    if (!e) return;
    char *p = e->data;
    while (*p) {
        if (isdigit(*p)) {
            int count = 0;
            while (isdigit(*p)) {
                count = count * 10 + (*p - '0');
                p++;
                if (count > 1000000) { // treat as invalid
                    printf("\n");
                    return;
                }
            }
            if (*p == 0) break;
            char ch = *p;
            for (int i = 0; i < count; i++) {
                putchar(ch);
            }
            p++;
        } else {
            p++;
        }
    }
    printf("\n");
}

void delete_encoded(char *name) {
    struct encoded *e = encodings, *prev = NULL;
    while (e) {
        if (strcmp(e->name, name) == 0) {
            if (prev) prev->next = e->next;
            else encodings = e->next;
            free(e->name);
            free(e->data);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void list_encoded() {
    struct encoded *e = encodings;
    while (e) {
        printf("%s\n", e->name);
        e = e->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], data[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, name, data);
        if (strcmp(cmd, "STORE") == 0 && n >= 2) {
            store_encoded(name, data);
        } else if (strcmp(cmd, "DECODE") == 0 && n >= 2) {
            decode_print(name);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_encoded(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    return 0;
}