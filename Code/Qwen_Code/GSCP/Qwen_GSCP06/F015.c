// F015.c
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
    char* note;
    struct Contact* next;
} Contact;

Contact* head = NULL;

Contact* find_contact(const char* name) {
    for (Contact* c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "ADD") == 0) {
            char* name = get_word(&p);
            char* email = get_word(&p);
            char* phone = p;
            Contact* n = malloc(sizeof(Contact));
            n->name = strdup(name);
            n->email = strdup(email);
            n->phone = strdup(phone);
            n->note = strdup("");
            n->next = head;
            head = n;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char* name = get_word(&p);
            Contact* c = find_contact(name);
            if (c) {
                free(c->note);
                c->note = strdup(p);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char* keep = get_word(&p);
            char* remove = p;
            Contact* k = find_contact(keep);
            Contact* r = find_contact(remove);
            if (k && r) {
                size_t len = strlen(k->note) + strlen(r->note) + 2;
                char* new_note = malloc(len);
                snprintf(new_note, len, "%s %s", k->note, r->note);
                free(k->note);
                k->note = new_note;
                
                Contact* prev = NULL;
                for (Contact* c = head; c; prev = c, c = c->next) {
                    if (c == r) {
                        if (prev) prev->next = c->next;
                        else head = c->next;
                        free(c->name);
                        free(c->email);
                        free(c->phone);
                        free(c->note);
                        free(c);
                        break;
                    }
                }
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
                    free(c->note);
                    free(c);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* name = get_word(&p);
            Contact* c = find_contact(name);
            if (c) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Contact* c = head; c; c = c->next) {
                printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
            }
        }
    }
    return 0;
}