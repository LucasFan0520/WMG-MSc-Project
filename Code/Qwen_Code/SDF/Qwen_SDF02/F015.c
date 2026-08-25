// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct Contact *next;
} Contact;

Contact *contacts = NULL;

Contact* find_contact(const char *name) {
    for (Contact *c = contacts; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
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
            char name[256], email[256], phone[256];
            if (sscanf(rest, "%255s %255s %255s", name, email, phone) != 3) continue;
            if (find_contact(name)) continue;
            Contact *c = malloc(sizeof(Contact));
            if (!c) continue;
            c->name = strdup(name);
            c->email = strdup(email);
            c->phone = strdup(phone);
            c->note = strdup("");
            c->next = contacts;
            contacts = c;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", name, &n) != 1) continue;
            char *text = rest + n;
            while (*text == ' ') text++;
            Contact *c = find_contact(name);
            if (c) {
                free(c->note);
                c->note = strdup(text);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256], remove[256];
            if (sscanf(rest, "%255s %255s", keep, remove) != 2) continue;
            Contact *k = find_contact(keep);
            Contact *r = find_contact(remove);
            if (k && r) {
                size_t len1 = strlen(k->note);
                size_t len2 = strlen(r->note);
                char *new_note = malloc(len1 + 1 + len2 + 1);
                if (new_note) {
                    memcpy(new_note, k->note, len1);
                    new_note[len1] = ' ';
                    memcpy(new_note + len1 + 1, r->note, len2 + 1);
                    free(k->note);
                    k->note = new_note;
                }
                Contact *prev = NULL;
                for (Contact *c = contacts; c; prev = c, c = c->next) {
                    if (c == r) {
                        if (prev) prev->next = c->next;
                        else contacts = c->next;
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
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Contact *prev = NULL;
            for (Contact *c = contacts; c; prev = c, c = c->next) {
                if (strcmp(c->name, name) == 0) {
                    if (prev) prev->next = c->next;
                    else contacts = c->next;
                    free(c->name);
                    free(c->email);
                    free(c->phone);
                    free(c->note);
                    free(c);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Contact *c = find_contact(name);
            if (c) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Contact *c = contacts; c; c = c->next)
                printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        }
    }
    while (contacts) {
        Contact *c = contacts;
        contacts = contacts->next;
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c->note);
        free(c);
    }
    return 0;
}