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

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[200];
        char email[200];
        char phone[200];
        if (sscanf(line, "%[^,],%[^,],%s", name, email, phone) == 3) {
            Contact *p = malloc(sizeof(Contact));
            p->name = strdup(name);
            p->email = strdup(email);
            p->phone = strdup(phone);
            p->next = head;
            head = p;
        }
    }
    fclose(f);
}

void add_contact(const char *csv) {
    char name[200];
    char email[200];
    char phone[200];
    if (sscanf(csv, "%[^,],%[^,],%s", name, email, phone) == 3) {
        Contact *p = malloc(sizeof(Contact));
        p->name = strdup(name);
        p->email = strdup(email);
        p->phone = strdup(phone);
        p->next = head;
        head = p;
    }
}

void delete_contact(const char *name) {
    Contact *p = head;
    Contact *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            Contact *next = p->next;
            free(p->name);
            free(p->email);
            free(p->phone);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void find_contact(const char *name) {
    for (Contact *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            printf("%s,%s,%s\n", p->name, p->email, p->phone);
            return;
        }
    }
}

void report_contacts() {
    for (Contact *p = head; p; p = p->next) {
        printf("%s,%s,%s\n", p->name, p->email, p->phone);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[1000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char arg[800];
        arg[0] = 0;
        sscanf(line, "%s %[^\n]", cmd, arg);
        if (strcmp(cmd, "ADD") == 0) {
            add_contact(arg);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_contact(arg);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_contact(arg);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    while (head) {
        Contact *n = head->next;
        free(head->name);
        free(head->email);
        free(head->phone);
        free(head);
        head = n;
    }
    return 0;
}