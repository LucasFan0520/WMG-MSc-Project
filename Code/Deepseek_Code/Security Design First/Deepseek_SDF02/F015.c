/* F015.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct contact {
    char *name;
    char *email;
    char *phone;
    char *note;
    struct contact *next;
} contact_t;

contact_t *head = NULL;
contact_t *tail = NULL;

contact_t *find_contact(const char *name) {
    contact_t *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    contact_t *c = malloc(sizeof(contact_t));
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
    if (tail) tail->next = c;
    else head = c;
    tail = c;
}

void note_contact(const char *name, const char *text) {
    contact_t *c = find_contact(name);
    if (!c) return;
    char *new = strdup(text);
    if (!new) return;
    free(c->note);
    c->note = new;
}

void merge_contacts(const char *keep, const char *remove) {
    contact_t *k = find_contact(keep);
    contact_t *r = find_contact(remove);
    if (!k || !r) return;
    // combine notes
    size_t newlen = strlen(k->note) + 1 + strlen(r->note) + 1;
    char *new = malloc(newlen);
    if (!new) return;
    sprintf(new, "%s %s", k->note, r->note);
    free(k->note);
    k->note = new;
    // delete remove
    contact_t *prev = NULL, *c = head;
    while (c) {
        if (c == r) {
            if (prev) prev->next = c->next;
            else head = c->next;
            if (c == tail) tail = prev;
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
    contact_t *prev = NULL, *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            if (c == tail) tail = prev;
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
    contact_t *c = find_contact(name);
    if (!c) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts(void) {
    contact_t *c = head;
    while (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
        c = c->next;
    }
}

void free_all(void) {
    contact_t *c = head;
    while (c) {
        contact_t *nxt = c->next;
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c->note);
        free(c);
        c = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256], c[256];
        if (sscanf(line, "%9s %255s %255s %255s", cmd, a, b, c) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                if (sscanf(line, "%*s %255s %255s %255s", a, b, c) == 3)
                    add_contact(a, b, c);
            } else if (strcmp(cmd, "NOTE") == 0) {
                note_contact(a, rest);
            } else if (strcmp(cmd, "MERGE") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    merge_contacts(a, b);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_contact(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_contact_print(a);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_contacts();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}