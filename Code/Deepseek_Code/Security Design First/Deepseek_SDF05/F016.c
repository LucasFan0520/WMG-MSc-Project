// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    struct Contact *next;
} Contact;

Contact *contacts = NULL;

Contact *find_contact(const char *name) {
    Contact *c = contacts;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_contact_csv(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    c->name = safe_strdup(name);
    c->email = safe_strdup(email);
    c->phone = safe_strdup(phone);
    c->next = contacts;
    contacts = c;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[512], email[512], phone[512];
        if (sscanf(line, "%511[^,],%511[^,],%511[^,]", name, email, phone) == 3) {
            add_contact_csv(name, email, phone);
        }
    }
    fclose(f);
}

void delete_contact(const char *name) {
    Contact *prev = NULL, *cur = contacts;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else contacts = cur->next;
            free(cur->name);
            free(cur->email);
            free(cur->phone);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_contact_cmd(const char *name) {
    Contact *c = find_contact(name);
    if (!c) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void report_contacts(void) {
    Contact *c = contacts;
    while (c) {
        printf("%s,%s,%s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void free_all(void) {
    Contact *c = contacts;
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
    if (argc >= 2) load_file(argv[1]);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512], arg3[512];
        int n = sscanf(line, "%31s %511s %511s %511s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            add_contact_csv(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_contact_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
        }
    }
    free_all();
    return 0;
}