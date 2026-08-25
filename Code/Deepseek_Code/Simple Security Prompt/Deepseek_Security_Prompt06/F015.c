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
    c->name = malloc(strlen(name) + 1);
    strcpy(c->name, name);
    c->email = malloc(strlen(email) + 1);
    strcpy(c->email, email);
    c->phone = malloc(strlen(phone) + 1);
    strcpy(c->phone, phone);
    c->note = malloc(1);
    c->note[0] = '\0';
    c->next = contacts;
    contacts = c;
}

void note_contact(const char *name, const char *text) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = malloc(strlen(text) + 1);
    strcpy(c->note, text);
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes
    size_t newlen = strlen(k->note) + strlen(r->note) + 2;
    char *new_note = malloc(newlen);
    sprintf(new_note, "%s %s", k->note, r->note);
    free(k->note);
    k->note = new_note;
    // delete remove
    Contact *c = contacts, *prev = NULL;
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
    Contact *c = contacts, *prev = NULL;
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

void free_all(void) {
    Contact *c = contacts;
    while (c) {
        Contact *tmp = c->next;
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c->note);
        free(c);
        c = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[256], d[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s %255s %255s", a, b, c) == 3) {
                add_contact(a, b, c);
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (sscanf(rest, "%255s", a) == 1) {
                const char *txt = rest + strlen(a) + 1;
                while (*txt == ' ') txt++;
                if (*txt) note_contact(a, txt);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(rest, "%255s %255s", a, b) == 2) merge_contacts(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", a) == 1) delete_contact(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", a) == 1) find_contact_print(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free_all();
    return 0;
}