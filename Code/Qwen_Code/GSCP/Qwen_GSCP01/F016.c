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

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    if (!c) exit(EXIT_FAILURE);
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = contacts;
    contacts = c;
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
    printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void report_contacts() {
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
        free(curr);
        curr = next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        if (name && email && phone) {
            add_contact(name, email, phone);
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        
        sscanf(line, "%31s %255[^,],%255[^,],%255s", cmd, arg1, arg2, arg3);
        
        if (strcmp(cmd, "ADD") == 0) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free_all();
    return 0;
}