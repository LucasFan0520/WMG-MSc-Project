// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Encoded {
    char *name;
    char *data;
    struct Encoded *next;
} Encoded;

Encoded *encoded_list = NULL;

Encoded *find_encoded(const char *name) {
    Encoded *curr = encoded_list;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void store_encoded(const char *name, const char *data) {
    if (find_encoded(name)) return;
    Encoded *e = malloc(sizeof(Encoded));
    if (!e) exit(EXIT_FAILURE);
    e->name = strdup(name);
    e->data = strdup(data);
    e->next = encoded_list;
    encoded_list = e;
}

void decode_encoded(const char *name) {
    Encoded *e = find_encoded(name);
    if (!e) return;
    
    char *p = e->data;
    while (*p) {
        if (!isdigit((unsigned char)*p)) return;
        char *endptr;
        long count = strtol(p, &endptr, 10);
        if (count <= 0 || count > 1000000) return;
        if (!*endptr) return;
        char ch = *endptr;
        for (long i = 0; i < count; i++) {
            if (ch == ' ') putchar('_');
            else putchar(ch);
        }
        p = endptr + 1;
    }
    putchar('\n');
}

void delete_encoded(const char *name) {
    Encoded *curr = encoded_list;
    Encoded *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else encoded_list = curr->next;
            free(curr->name);
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_encoded() {
    Encoded *curr = encoded_list;
    while (curr) {
        printf("%s\n", curr->name);
        curr = curr->next;
    }
}

void free_all() {
    Encoded *curr = encoded_list;
    while (curr) {
        Encoded *next = curr->next;
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
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, arg1, arg2);
        
        if (strcmp(cmd, "STORE") == 0 && matched >= 2) {
            store_encoded(arg1, arg2);
        } else if (strcmp(cmd, "DECODE") == 0) {
            decode_encoded(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_encoded(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_encoded();
        }
    }
    free_all();
    return 0;
}