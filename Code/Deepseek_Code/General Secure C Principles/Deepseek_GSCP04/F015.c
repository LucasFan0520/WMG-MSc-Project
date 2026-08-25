/* F015.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

static Contact *contacts = NULL;
static size_t ccount = 0, ccap = 0;

static Contact *find_contact(const char *name) {
    for (size_t i = 0; i < ccount; i++) {
        if (strcmp(contacts[i].name, name) == 0)
            return &contacts[i];
    }
    return NULL;
}

static void add_contact(const char *name, const char *email, const char *phone) {
    if (ccount == ccap) {
        ccap = ccap ? ccap * 2 : 4;
        contacts = realloc(contacts, ccap * sizeof(Contact));
        if (!contacts) { perror("realloc"); exit(1); }
    }
    contacts[ccount].name = strdup(name);
    contacts[ccount].email = strdup(email);
    contacts[ccount].phone = strdup(phone);
    contacts[ccount].note = strdup("");
    ccount++;
}

static void set_note(const char *name, const char *note) {
    Contact *c = find_contact(name);
    if (c) {
        free(c->note);
        c->note = strdup(note);
    }
}

static void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine note fields with space
    size_t newlen = strlen(k->note) + 1 + strlen(r->note) + 1;
    char *combined = malloc(newlen);
    if (!combined) { perror("malloc"); exit(1); }
    sprintf(combined, "%s %s", k->note, r->note);
    free(k->note);
    k->note = combined;
    // delete removed contact
    delete_contact(remove);
}

static void delete_contact(const char *name) {
    for (size_t i = 0; i < ccount; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            free(contacts[i].name);
            free(contacts[i].email);
            free(contacts[i].phone);
            free(contacts[i].note);
            for (size_t j = i; j < ccount - 1; j++)
                contacts[j] = contacts[j + 1];
            ccount--;
            return;
        }
    }
}

static void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

static void list_contacts(void) {
    for (size_t i = 0; i < ccount; i++) {
        printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *email = strtok(NULL, " ");
            char *phone = strtok(NULL, " ");
            if (!name || !email || !phone) continue;
            if (find_contact(name)) {
                // update fields? Spec says ADD creates, maybe replace? We'll update.
                Contact *c = find_contact(name);
                free(c->email); c->email = strdup(email);
                free(c->phone); c->phone = strdup(phone);
            } else {
                add_contact(name, email, phone);
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            set_note(name, note);
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
    for (size_t i = 0; i < ccount; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}