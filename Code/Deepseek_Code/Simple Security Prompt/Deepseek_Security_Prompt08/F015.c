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

Contact *head = NULL, *tail = NULL;

Contact *find_contact(const char *name) {
    Contact *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    Contact *c = find_contact(name);
    if (c) {
        free(c->email);
        free(c->phone);
        c->email = malloc(strlen(email) + 1);
        strcpy(c->email, email);
        c->phone = malloc(strlen(phone) + 1);
        strcpy(c->phone, phone);
        return;
    }
    Contact *new = malloc(sizeof(Contact));
    new->name = malloc(strlen(name) + 1);
    strcpy(new->name, name);
    new->email = malloc(strlen(email) + 1);
    strcpy(new->email, email);
    new->phone = malloc(strlen(phone) + 1);
    strcpy(new->phone, phone);
    new->note = NULL;
    new->next = NULL;
    if (tail) {
        tail->next = new;
        tail = new;
    } else {
        head = tail = new;
    }
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
    if (r->note) {
        if (k->note) {
            char *new_note = malloc(strlen(k->note) + strlen(r->note) + 1);
            strcpy(new_note, k->note);
            strcat(new_note, r->note);
            free(k->note);
            k->note = new_note;
        } else {
            k->note = strdup(r->note);
        }
    }
    Contact *prev = NULL, *curr = head;
    while (curr) {
        if (curr == r) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
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
    Contact *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
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
    printf("%s %s %s", c->name, c->email, c->phone);
    if (c->note) printf(" %s", c->note);
    putchar('\n');
}

void list_contacts(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s", c->name, c->email, c->phone);
        if (c->note) printf(" %s", c->note);
        putchar('\n');
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *email = strtok(NULL, " ");
            char *phone = strtok(NULL, " ");
            if (name && email && phone) add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (name && text) note_contact(name, text);
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
    free(line);
    free_all();
    return 0;
}