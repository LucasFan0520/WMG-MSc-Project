/* F016.c */
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

Contact *find_contact(const char *name) {
    Contact *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    c->next = head;
    head = c;
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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

void find_contact_cmd(const char *name) {
    Contact *c = find_contact(name);
    if (!c) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void free_all(void) {
    Contact *c = head;
    while (c) {
        Contact *tmp = c;
        c = c->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char name[256], email[256], phone[256];
                if (sscanf(line, "%255[^,],%255[^,],%255s", name, email, phone) == 3) {
                    add_contact(name, email, phone);
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(line + 4, "%255[^,],%255[^,],%255s", name, email, phone) == 3) {
                add_contact(name, email, phone);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_contact(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_contact_cmd(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free_all();
    return 0;
}