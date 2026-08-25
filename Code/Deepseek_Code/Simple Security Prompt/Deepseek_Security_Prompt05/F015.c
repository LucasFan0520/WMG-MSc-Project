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
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->note = strdup("");
    c->next = contacts;
    contacts = c;
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
    size_t newlen = strlen(k->note) + strlen(r->note) + 2;
    char *combined = malloc(newlen);
    if (!combined) return;
    sprintf(combined, "%s %s", k->note, r->note);
    free(k->note);
    k->note = combined;
    delete_contact(remove);
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *txt = line + strlen(cmd) + strlen(arg1) + 2;
            while (*txt == ' ') txt++;
            note_contact(arg1, txt);
        } else if (strcmp(cmd, "MERGE") == 0 && n == 3) {
            merge_contacts(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    while (contacts) {
        Contact *tmp = contacts;
        contacts = contacts->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp->note);
        free(tmp);
    }
    return 0;
}