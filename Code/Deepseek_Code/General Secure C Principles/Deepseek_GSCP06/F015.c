// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct Contact *next;
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

void set_note(const char *name, const char *note) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = strdup(note);
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes: keep note + " " + remove note
    char *new_note = malloc(strlen(k->note) + strlen(r->note) + 2);
    sprintf(new_note, "%s %s", k->note, r->note);
    free(k->note);
    k->note = new_note;
    // delete r
    Contact *cur = contacts, *prev = NULL;
    while (cur) {
        if (cur == r) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(r->name);
    free(r->email);
    free(r->phone);
    free(r->note);
    free(r);
}

void delete_contact(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    Contact *cur = contacts, *prev = NULL;
    while (cur) {
        if (cur == c) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(c->name);
    free(c->email);
    free(c->phone);
    free(c->note);
    free(c);
}

void find_contact_cmd(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        c = c->next;
    }
}

void cleanup_contacts(void) {
    Contact *c = contacts;
    while (c) {
        Contact *tmp = c;
        c = c->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], arg2[256], arg3[256], rest[2048];
        int n = sscanf(line, "%31s %255s %255s %255s %2047s", cmd, name, arg2, arg3, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            add_contact(name, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *note = line + strlen(cmd) + 1 + strlen(name) + 1;
            set_note(name, note);
        } else if (strcmp(cmd, "MERGE") == 0 && n >= 3) {
            merge_contacts(name, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact_cmd(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    cleanup_contacts();
    return 0;
}