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

Contact *head = NULL;

void add_contact(const char *name, const char *email, const char *phone) {
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = head;
    head = c;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[1024], email[1024], phone[1024];
        if (sscanf(line, "%1023[^,],%1023[^,],%1023s", name, email, phone) == 3) {
            add_contact(name, email, phone);
        }
    }
    fclose(f);
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
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_contact(const char *name) {
    for (Contact *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            printf("%s %s %s\n", curr->name, curr->email, curr->phone);
            return;
        }
    }
}

void report_contacts() {
    for (Contact *curr = head; curr; curr = curr->next) {
        printf("%s %s %s\n", curr->name, curr->email, curr->phone);
    }
}

void free_all() {
    Contact *curr = head;
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
        
        char cmd[16];
        char arg1[1024], arg2[1024], arg3[1024];
        
        int n = sscanf(line, "%15s %1023[^,],%1023[^,],%1023s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_contact(arg1, arg2, arg3);
        } else if (sscanf(line, "%15s %1023s", cmd, arg1) >= 2) {
            if (strcmp(cmd, "DELETE") == 0) delete_contact(arg1);
            else if (strcmp(cmd, "FIND") == 0) find_contact(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free_all();
    return 0;
}