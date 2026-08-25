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

Contact *find_contact(const char *name) {
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
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->note = strdup("");
    c->next = NULL;
    if (!c->name || !c->email || !c->phone || !c->note) {
        free(c->name); free(c->email); free(c->phone); free(c->note); free(c);
        return;
    }
    if (!head) head = c;
    else {
        Contact *last = head;
        while (last->next) last = last->next;
        last->next = c;
    }
}

void set_note(const char *name, const char *note) {
    Contact *c = find_contact(name);
    if (!c) return;
    char *newnote = strdup(note);
    if (!newnote) return;
    free(c->note);
    c->note = newnote;
}

void merge_contact(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes: keep's note + space + remove's note
    size_t len = strlen(k->note) + 1 + strlen(r->note) + 1;
    char *combined = malloc(len);
    if (!combined) return;
    if (strlen(k->note) > 0)
        snprintf(combined, len, "%s %s", k->note, r->note);
    else
        snprintf(combined, len, "%s", r->note);
    free(k->note);
    k->note = combined;
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
            break;
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

void list_contacts(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        c = c->next;
    }
}

void free_all(void) {
    Contact *c = head;
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], email[256], phone[256], text[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", name, email, phone) == 3)
                add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, text) == 2)
                set_note(name, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, email) == 2) // email as remove name
                merge_contact(name, email);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_contact_print(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free(line);
    free_all();
    return 0;
}