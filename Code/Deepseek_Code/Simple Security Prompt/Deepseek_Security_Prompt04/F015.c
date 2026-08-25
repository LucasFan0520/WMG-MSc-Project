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

Contact *find_contact(char *name) {
    for (Contact *c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

void add_contact(char *name, char *email, char *phone) {
    Contact *c = find_contact(name);
    if (c) {
        free(c->email);
        free(c->phone);
        c->email = strdup(email);
        c->phone = strdup(phone);
        return;
    }
    Contact *new = malloc(sizeof(Contact));
    new->name = strdup(name);
    new->email = strdup(email);
    new->phone = strdup(phone);
    new->note = NULL; // no note initially
    new->next = head;
    head = new;
}

void note_contact(char *name, char *text) {
    Contact *c = find_contact(name);
    if (!c) return;
    free(c->note);
    c->note = strdup(text);
}

void merge_contacts(char *keep, char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    if (k->note && r->note) {
        char *merged = malloc(strlen(k->note) + 1 + strlen(r->note) + 1);
        sprintf(merged, "%s %s", k->note, r->note);
        free(k->note);
        k->note = merged;
    } else if (r->note && !k->note) {
        k->note = strdup(r->note);
    }
    // delete r
    Contact *prev = NULL;
    for (Contact *c = head; c; c = c->next) {
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
    }
}

void delete_contact(char *name) {
    Contact *prev = NULL;
    for (Contact *c = head; c; c = c->next) {
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
    }
}

void find_contact_print(char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s", c->name, c->email, c->phone);
    if (c->note) printf(" %s", c->note);
    putchar('\n');
}

void list_all(void) {
    for (Contact *c = head; c; c = c->next) {
        printf("%s %s %s", c->name, c->email, c->phone);
        if (c->note) printf(" %s", c->note);
        putchar('\n');
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0 && n == 3) {
            note_contact(arg1, arg2);
        } else if (strcmp(cmd, "MERGE") == 0 && n == 3) {
            merge_contacts(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_contact_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_all();
        }
    }
    free(line);
    free_all();
    return 0;
}