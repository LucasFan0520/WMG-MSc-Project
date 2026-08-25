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
    Contact *curr = contacts;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
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

void note_contact(const char *name, const char *text) {
    Contact *c = find_contact(name);
    if (c) {
        free(c->note);
        c->note = strdup(text);
    }
}

void merge_contacts(const char *keep, const char *remove) {
    Contact *ck = find_contact(keep);
    Contact *cr = find_contact(remove);
    if (!ck || !cr) return;
    
    size_t len = strlen(ck->note) + strlen(cr->note) + 2;
    char *merged = malloc(len);
    snprintf(merged, len, "%s %s", ck->note, cr->note);
    free(ck->note);
    ck->note = merged;
    
    Contact *curr = contacts, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, remove) == 0) {
            if (prev) prev->next = curr->next;
            else contacts = curr->next;
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

void delete_contact(const char *name) {
    Contact *curr = contacts, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else contacts = curr->next;
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

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (c) printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts() {
    Contact *curr = contacts;
    while (curr) {
        printf("%s %s %s %s\n", curr->name, curr->email, curr->phone, curr->note);
        curr = curr->next;
    }
}

void free_all() {
    Contact *curr = contacts;
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
        
        char cmd[32], arg1[256], arg2[256], arg3[256], text[4096] = {0};
        int n = sscanf(line, "%31s %255s %255s %255s %4095[^\n]", cmd, arg1, arg2, arg3, text);
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) add_contact(arg1, arg2, arg3);
        else if (strcmp(cmd, "NOTE") == 0 && n >= 2) note_contact(arg1, arg2);
        else if (strcmp(cmd, "MERGE") == 0 && n >= 2) merge_contacts(arg1, arg2);
        else if (strcmp(cmd, "DELETE") == 0 && n >= 1) delete_contact(arg1);
        else if (strcmp(cmd, "FIND") == 0 && n >= 1) find_contact_print(arg1);
        else if (strcmp(cmd, "LIST") == 0) list_contacts();
    }
    free_all();
    return 0;
}