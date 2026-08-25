// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct contact {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct contact *next;
};

struct contact *contacts = NULL;

struct contact *find_contact(char *name) {
    struct contact *c = contacts;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(char *name, char *email, char *phone) {
    struct contact *c = find_contact(name);
    if (c) {
        free(c->email);
        free(c->phone);
        c->email = malloc(strlen(email) + 1);
        strcpy(c->email, email);
        c->phone = malloc(strlen(phone) + 1);
        strcpy(c->phone, phone);
    } else {
        c = malloc(sizeof(struct contact));
        c->name = malloc(strlen(name) + 1);
        strcpy(c->name, name);
        c->email = malloc(strlen(email) + 1);
        strcpy(c->email, email);
        c->phone = malloc(strlen(phone) + 1);
        strcpy(c->phone, phone);
        c->note = malloc(1);
        c->note[0] = 0;
        c->next = contacts;
        contacts = c;
    }
}

void note_contact(char *name, char *text) {
    struct contact *c = find_contact(name);
    if (c) {
        free(c->note);
        c->note = malloc(strlen(text) + 1);
        strcpy(c->note, text);
    }
}

void merge_contacts(char *keep, char *remove) {
    struct contact *k = find_contact(keep);
    struct contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes
    char *new_note = malloc(strlen(k->note) + strlen(r->note) + 2);
    sprintf(new_note, "%s %s", k->note, r->note);
    free(k->note);
    k->note = new_note;
    // delete remove
    struct contact *c = contacts, *prev = NULL;
    while (c) {
        if (c == r) {
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

void delete_contact(char *name) {
    struct contact *c = contacts, *prev = NULL;
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

void find_contact_print(char *name) {
    struct contact *c = find_contact(name);
    if (c) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts() {
    struct contact *c = contacts;
    while (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        c = c->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[256], arg4[4096];
        int n = sscanf(line, "%31s %255s %255s %255s %4095[^\n]", cmd, arg1, arg2, arg3, arg4);
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            note_contact(arg1, rest);
        } else if (strcmp(cmd, "MERGE") == 0 && n >= 3) {
            merge_contacts(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_contact_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    return 0;
}