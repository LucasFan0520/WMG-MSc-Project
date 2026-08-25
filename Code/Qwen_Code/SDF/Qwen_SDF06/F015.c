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
    for (Contact *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) return curr;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->note = strdup("");
    c->next = head;
    head = c;
}

void note_contact(const char *name, const char *text) {
    Contact *c = find_contact(name);
    if (c) {
        free(c->note);
        c->note = strdup(text);
    }
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (k && r) {
        size_t len = strlen(k->note) + strlen(r->note) + 2;
        char *new_note = malloc(len);
        if (!new_note) return;
        snprintf(new_note, len, "%s %s", k->note, r->note);
        free(k->note);
        k->note = new_note;
        
        Contact *curr = head;
        Contact *prev = NULL;
        while (curr) {
            if (curr == r) {
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

void delete_contact(const char *name) {
    Contact *curr = head;
    Contact *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            Contact *next = curr->next;
            free(curr->name);
            free(curr->email);
            free(curr->phone);
            free(curr->note);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (c) {
        printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
    }
}

void list_contacts() {
    for (Contact *curr = head; curr; curr = curr->next) {
        printf("%s %s %s %s\n", curr->name, curr->email, curr->phone, curr->note);
    }
}

void free_all() {
    Contact *curr = head;
    while (curr) {
        Contact *next = curr->next;
        free(curr->name);
        free(curr->email);
        free(curr->phone);
        free(curr->note);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char arg1[1024], arg2[1024], arg3[1024];
        
        int n = sscanf(line, "%15s %1023s %1023s %1023[^\n]", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0 && n >= 3) {
            note_contact(arg1, arg3);
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
    free_all();
    return 0;
}