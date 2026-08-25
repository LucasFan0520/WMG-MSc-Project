/*
 * F015.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct contact {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct contact *next;
} Contact;

Contact *contacts = NULL;

Contact *find_contact(const char *name) {
    Contact *c = contacts;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->note = strdup("");
    c->next = contacts;
    contacts = c;
}

void note_contact(const char *name, const char *note) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = strdup(note);
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes
    char *new_note = malloc(strlen(k->note) + strlen(r->note) + 2);
    sprintf(new_note, "%s %s", k->note, r->note);
    free(k->note);
    k->note = new_note;
    // delete r
    Contact *prev = NULL, *c = contacts;
    while (c) {
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
        prev = c;
        c = c->next;
    }
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *c = contacts;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else contacts = c->next;
            free(c->name);
            free(c->email);
            free(c->phone);
            free(c->note);
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void find_contact_cmd(const char *name) {
    Contact *c = find_contact(name);
    if (!c) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        c = c->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3) >= 3) {
            if (strcmp(cmd, "ADD") == 0) {
                add_contact(arg1, arg2, arg3);
            } else if (strcmp(cmd, "NOTE") == 0) {
                char *rest = line + strlen(cmd) + strlen(arg1) + 1;
                while (*rest == ' ') rest++;
                note_contact(arg1, rest);
            } else if (strcmp(cmd, "MERGE") == 0) {
                merge_contacts(arg1, arg2);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_contact(arg1);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_contact_cmd(arg1);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_contacts();
            }
        }
    }
    return 0;
}