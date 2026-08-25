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

Contact *head = NULL;

Contact* find_contact(const char *name) {
    Contact *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void free_contact(Contact *c) {
    free(c->name);
    free(c->email);
    free(c->phone);
    free(c->note);
    free(c);
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *new = malloc(sizeof(Contact));
    new->name = strdup(name);
    new->email = strdup(email);
    new->phone = strdup(phone);
    new->note = strdup("");
    new->next = NULL;
    if (!head) head = new;
    else {
        Contact *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void note_contact(const char *name, const char *text) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = strdup(text);
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes: keep's note + " " + remove's note?
    // We'll concatenate with space.
    size_t len = strlen(k->note) + strlen(r->note) + 2;
    char *new_note = malloc(len);
    sprintf(new_note, "%s %s", k->note, r->note);
    free(k->note);
    k->note = new_note;
    // delete removed contact
    Contact *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, remove) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free_contact(c);
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
            free_contact(c);
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *email = strtok(NULL, " ");
            char *phone = strtok(NULL, " ");
            if (name && email && phone) add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (name && text) note_contact(name, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = strtok(NULL, " ");
            char *remove = strtok(NULL, " ");
            if (keep && remove) merge_contacts(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_contact_print(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    while (head) {
        Contact *tmp = head;
        head = head->next;
        free_contact(tmp);
    }
    return 0;
}