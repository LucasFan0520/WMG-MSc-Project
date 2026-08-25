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
    if (!c) exit(EXIT_FAILURE);
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
    
    size_t len1 = strlen(k->note);
    size_t len2 = strlen(r->note);
    char *merged = malloc(len1 + len2 + 2);
    if (!merged) exit(EXIT_FAILURE);
    strcpy(merged, k->note);
    if (len1 > 0 && len2 > 0) {
        merged[len1] = ' ';
        strcpy(merged + len1 + 1, r->note);
    } else {
        strcpy(merged + len1, r->note);
    }
    free(k->note);
    k->note = merged;
    
    Contact *curr = contacts;
    Contact *prev = NULL;
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
    Contact *curr = contacts;
    Contact *prev = NULL;
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
    if (!c) return;
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void list_contacts() {
    Contact *curr = contacts;
    while (curr) {
        find_contact_print(curr->name);
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
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        char arg4[3200] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, arg4);
        
        if (strcmp(cmd, "ADD") == 0 && matched >= 3) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0 && matched >= 3) {
            char *text_start = strchr(line, ' ');
            if (text_start) {
                text_start = strchr(text_start + 1, ' ');
                if (text_start) note_contact(arg1, text_start + 1);
            }
        } else if (strcmp(cmd, "MERGE") == 0 && matched >= 2) {
            merge_contacts(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }
    free_all();
    return 0;
}