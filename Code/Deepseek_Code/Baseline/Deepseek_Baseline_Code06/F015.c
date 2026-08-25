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

Contact *head = NULL;

Contact* find_contact(const char *name) {
    Contact *c = head;
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
    c->next = head;
    head = c;
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
    strcpy(new_note, k->note);
    if (strlen(k->note) > 0 && strlen(r->note) > 0) strcat(new_note, " ");
    strcat(new_note, r->note);
    free(k->note);
    k->note = new_note;
    // delete remove
    Contact *c = head, *prev = NULL;
    while (c) {
        if (c == r) {
            if (prev) prev->next = c->next;
            else head = c->next;
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

void delete_contact(const char *name) {
    Contact *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
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

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts() {
    Contact *c = head;
    while (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        c = c->next;
    }
}

void free_all() {
    while (head) {
        Contact *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char name[256], other[256], rest[4096];
        int n = sscanf(line, "%31s %255s %255s %[^\n]", cmd, name, other, rest);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_contact(name, other, rest);
        } else if (strcmp(cmd, "NOTE") == 0 && n == 3) {
            note_contact(name, other);
        } else if (strcmp(cmd, "MERGE") == 0 && n == 3) {
            merge_contacts(name, other);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_contact_print(name);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_contacts();
        }
    }
    free_all();
    return 0;
}