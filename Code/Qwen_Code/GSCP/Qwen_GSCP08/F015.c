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

Contact *find_contact(char *name) {
    for (Contact *c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

void add_contact(char *name, char *email, char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->note = strdup("");
    c->next = head;
    head = c;
}

void set_note(char *name, char *text) {
    Contact *c = find_contact(name);
    if (c) {
        free(c->note);
        c->note = strdup(text);
    }
}

void merge_contacts(char *keep, char *remove) {
    Contact *ck = find_contact(keep);
    Contact *cr = find_contact(remove);
    if (ck && cr) {
        char *buf = malloc(strlen(ck->note) + strlen(cr->note) + 1);
        sprintf(buf, "%s%s", ck->note, cr->note);
        free(ck->note);
        ck->note = buf;
        Contact *prev = NULL;
        Contact *curr = head;
        while (curr) {
            if (curr == cr) {
                if (prev) prev->next = curr->next;
                else head = curr->next;
                free(curr->name);
                free(curr->email);
                free(curr->phone);
                free(curr->note);
                free(curr);
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }
}

void delete_contact(char *name) {
    Contact *prev = NULL;
    Contact *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->email);
            free(curr->phone);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_contact_info(char *name) {
    Contact *c = find_contact(name);
    if (c) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts() {
    for (Contact *c = head; c; c = c->next) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
    }
}

int main() {
    char cmd[20], a[100], b[100], c[100], note[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s%99s%99s", a, b, c);
            add_contact(a, b, c);
        } else if (strcmp(cmd, "NOTE") == 0) {
            scanf("%99s", a);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            set_note(a, note);
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%99s%99s", a, b);
            merge_contacts(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_contact(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", a);
            find_contact_info(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    return 0;
}