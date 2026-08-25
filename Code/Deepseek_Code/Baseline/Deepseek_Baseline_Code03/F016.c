// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct contact {
    char *name;
    char *email;
    char *phone;
    struct contact *next;
};

struct contact *contacts = NULL;

struct contact *find_contact(char *name) {
    struct contact *c = contacts;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(char *name, char *email, char *phone) {
    struct contact *c = find_contact(name);
    if (c) {
        free(c->email);
        free(c->phone);
        c->email = malloc(strlen(email) + 1);
        strcpy(c->email, email);
        c->phone = malloc(strlen(phone) + 1);
        strcpy(c->phone, phone);
    } else {
        c = malloc(sizeof(struct contact));
        c->name = malloc(strlen(name) + 1);
        strcpy(c->name, name);
        c->email = malloc(strlen(email) + 1);
        strcpy(c->email, email);
        c->phone = malloc(strlen(phone) + 1);
        strcpy(c->phone, phone);
        c->next = contacts;
        contacts = c;
    }
}

void delete_contact(char *name) {
    struct contact *c = contacts, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else contacts = c->next;
            free(c->name);
            free(c->email);
            free(c->phone);
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void find_contact_print(char *name) {
    struct contact *c = find_contact(name);
    if (c) printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_contacts() {
    struct contact *c = contacts;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[4096];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = 0;
            char name[256], email[256], phone[256];
            if (sscanf(line, "%255[^,],%255[^,],%255[^,]", name, email, phone) == 3) {
                add_contact(name, email, phone);
            }
        }
        fclose(f);
    }
    char cmd[32];
    char arg1[256], arg2[256], arg3[256];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        int n = sscanf(line, "%31s %255[^,],%255[^,],%255[^,]", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            sscanf(line, "%31s %255s", cmd, arg1);
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            sscanf(line, "%31s %255s", cmd, arg1);
            find_contact_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    return 0;
}