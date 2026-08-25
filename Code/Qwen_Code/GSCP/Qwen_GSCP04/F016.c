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

Contact *contacts = NULL;

Contact *find_contact(const char *name) {
    Contact *curr = contacts;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        if (name && email && phone) {
            Contact *c = malloc(sizeof(Contact));
            c->name = strdup(name);
            c->email = strdup(email);
            c->phone = strdup(phone);
            c->next = contacts;
            contacts = c;
        }
    }
    fclose(f);
}

void add_contact(const char *csv) {
    char buf[4096];
    strncpy(buf, csv, sizeof(buf));
    char *name = strtok(buf, ",");
    char *email = strtok(NULL, ",");
    char *phone = strtok(NULL, ",");
    if (name && email && phone && !find_contact(name)) {
        Contact *c = malloc(sizeof(Contact));
        c->name = strdup(name);
        c->email = strdup(email);
        c->phone = strdup(phone);
        c->next = contacts;
        contacts = c;
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

void report_contacts() {
    Contact *curr = contacts;
    while (curr) {
        printf("%s,%s,%s\n", curr->name, curr->email, curr->phone);
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
        free(curr);
        curr = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], arg[4096] = {0};
        sscanf(line, "%31s %4095[^\n]", cmd, arg);
        
        if (strcmp(cmd, "ADD") == 0) add_contact(arg);
        else if (strcmp(cmd, "DELETE") == 0) delete_contact(arg);
        else if (strcmp(cmd, "FIND") == 0) find_contact_print(arg);
        else if (strcmp(cmd, "REPORT") == 0) report_contacts();
    }
    free_all();
    return 0;
}