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
    char *new_note;
    if (k->note[0] == '\0' && r->note[0] == '\0') {
        new_note = strdup("");
    } else if (k->note[0] == '\0') {
        new_note = strdup(r->note);
    } else if (r->note[0] == '\0') {
        new_note = strdup(k->note);
    } else {
        new_note = malloc(strlen(k->note) + 1 + strlen(r->note) + 1);
        sprintf(new_note, "%s %s", k->note, r->note);
    }
    free(k->note);
    k->note = new_note;
    // delete remove contact
    Contact *prev = NULL;
    Contact *c = contacts;
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

void delete_contact(const char *name) {
    Contact *prev = NULL;
    Contact *c = contacts;
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

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s:", c->name, c->email, c->phone);
    for (char *s = c->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void list_contacts() {
    Contact *c = contacts;
    while (c) {
        printf("%s %s %s:", c->name, c->email, c->phone);
        for (char *s = c->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        c = c->next;
    }
}

void free_all() {
    Contact *c = contacts;
    while (c) {
        Contact *next = c->next;
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c->note);
        free(c);
        c = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(rest, "%255s %255s %255s", name, email, phone) == 3) {
                add_contact(name, email, phone);
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s %[^\n]", name, note) == 2) {
                set_note(name, note);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256], remove[256];
            if (sscanf(rest, "%255s %255s", keep, remove) == 2) {
                merge_contacts(keep, remove);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                delete_contact(name);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                find_contact_print(name);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free_all();
    return 0;
}