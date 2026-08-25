// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
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

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        if (name && email && phone) {
            Contact *new = malloc(sizeof(Contact));
            new->name = malloc(strlen(name) + 1);
            strcpy(new->name, name);
            new->email = malloc(strlen(email) + 1);
            strcpy(new->email, email);
            new->phone = malloc(strlen(phone) + 1);
            strcpy(new->phone, phone);
            new->next = NULL;
            if (tail) { tail->next = new; tail = new; }
            else head = tail = new;
        }
    }
    free(line);
    fclose(f);
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
    new->next = NULL;
    if (tail) { tail->next = new; tail = new; }
    else head = tail = new;
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
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (c) printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    Contact *c = head;
    while (c) {
        printf("%s,%s,%s\n", c->name, c->email, c->phone);
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
        free(c);
        c = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, ",");
            char *email = strtok(NULL, ",");
            char *phone = strtok(NULL, ",");
            if (name && email && phone) add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free(line);
    free_all();
    return 0;
}