// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Contact {
    char* name;
    char* email;
    char* phone;
    struct Contact* next;
} Contact;

Contact* head = NULL;

Contact* find_contact(const char* name) {
    for (Contact* c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

void add_contact(const char* name, const char* email, const char* phone) {
    Contact* n = malloc(sizeof(Contact));
    n->name = strdup(name);
    n->email = strdup(email);
    n->phone = strdup(phone);
    n->next = head;
    head = n;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* f = fopen(argv[1], "r");
        if (f) {
            char l[MAX_LINE];
            while (fgets(l, sizeof(l), f)) {
                l[strcspn(l, "\n")] = 0;
                char* name = strtok(l, ",");
                char* email = strtok(NULL, ",");
                char* phone = strtok(NULL, ",");
                if (name && email && phone) {
                    add_contact(name, email, phone);
                }
            }
            fclose(f);
        }
    }
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "ADD") == 0) {
            char* csv = p;
            char* name = strtok(csv, ",");
            char* email = strtok(NULL, ",");
            char* phone = strtok(NULL, ",");
            if (name && email && phone) {
                add_contact(name, email, phone);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* name = get_word(&p);
            Contact* prev = NULL;
            for (Contact* c = head; c; prev = c, c = c->next) {
                if (strcmp(c->name, name) == 0) {
                    if (prev) prev->next = c->next;
                    else head = c->next;
                    free(c->name);
                    free(c->email);
                    free(c->phone);
                    free(c);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* name = get_word(&p);
            Contact* c = find_contact(name);
            if (c) printf("%s,%s,%s\n", c->name, c->email, c->phone);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Contact* c = head; c; c = c->next) {
                printf("%s,%s,%s\n", c->name, c->email, c->phone);
            }
        }
    }
    return 0;
}