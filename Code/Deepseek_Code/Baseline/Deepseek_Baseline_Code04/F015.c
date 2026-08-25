/* F015.c */
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
    if (c) {
        free(c->note);
        c->note = strdup(note);
    }
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes: keep's note + space + remove's note
    char *new_note = malloc(strlen(k->note) + strlen(r->note) + 2);
    sprintf(new_note, "%s %s", k->note, r->note);
    free(k->note);
    k->note = new_note;
    // delete remove
    Contact *prev = NULL, *cur = contacts;
    while (cur) {
        if (cur == r) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
            free(cur->name);
            free(cur->email);
            free(cur->phone);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *cur = contacts;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
            free(cur->name);
            free(cur->email);
            free(cur->phone);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_contact(Contact *c) {
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s\n", c->name);
        c = c->next;
    }
}

void free_contacts(void) {
    while (contacts) {
        Contact *tmp = contacts;
        contacts = contacts->next;
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], email[256], phone[256];
        int n = sscanf(line, "%s %s %s %s", cmd, name, email, phone);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 4) continue;
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            if (*p) set_note(name, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (n < 3) continue;
            char keep[256], rem[256];
            sscanf(line, "%*s %s %s", keep, rem);
            merge_contacts(keep, rem);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            Contact *c = find_contact(name);
            if (c) print_contact(c);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free_contacts();
    return 0;
}