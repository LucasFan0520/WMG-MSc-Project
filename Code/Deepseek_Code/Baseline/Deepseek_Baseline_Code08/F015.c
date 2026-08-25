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

Contact* find_contact(const char *name) {
    Contact *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *new = malloc(sizeof(Contact));
    new->name = strdup(name);
    new->email = strdup(email);
    new->phone = strdup(phone);
    new->note = strdup("");
    new->next = NULL;
    if (!head) head = new;
    else {
        Contact *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
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
    // merge note: concatenate
    size_t newlen = strlen(k->note) + strlen(r->note) + 2;
    char *new_note = malloc(newlen);
    sprintf(new_note, "%s %s", k->note, r->note);
    free(k->note);
    k->note = new_note;
    // delete r
    Contact *prev = NULL, *cur = head;
    while (cur) {
        if (cur == r) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur->email);
            free(cur->phone);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur->email);
            free(cur->phone);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_contact(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s %s\n", c->name, c->email, c->phone, c->note);
}

void print_all() {
    Contact *cur = head;
    while (cur) {
        printf("%s %s %s %s\n", cur->name, cur->email, cur->phone, cur->note);
        cur = cur->next;
    }
}

void free_all() {
    Contact *cur = head;
    while (cur) {
        Contact *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *email = strtok(NULL, " ");
            if (!email) continue;
            char *phone = strtok(NULL, " ");
            if (!phone) continue;
            add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            set_note(name, note);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = strtok(NULL, " ");
            if (!keep) continue;
            char *remove = strtok(NULL, " ");
            if (!remove) continue;
            merge_contacts(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) print_contact(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}