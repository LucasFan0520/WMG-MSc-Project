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
    c->name = malloc(strlen(name) + 1);
    strcpy(c->name, name);
    c->email = malloc(strlen(email) + 1);
    strcpy(c->email, email);
    c->phone = malloc(strlen(phone) + 1);
    strcpy(c->phone, phone);
    c->next = head;
    head = c;
}

void delete_contact(const char *name) {
    Contact *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
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

void find_contact_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s %s %s\n", c->name, c->email, c->phone);
}

void report_all(void) {
    Contact *c = head;
    while (c) {
        printf("%s %s %s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

void load_file(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (!fp) return;
    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], email[256], phone[256];
        if (sscanf(line, "%255[^,],%255[^,],%255[^,]", name, email, phone) == 3) {
            add_contact(name, email, phone);
        }
    }
    fclose(fp);
}

void free_all(void) {
    Contact *c = head;
    while (c) {
        Contact *tmp = c->next;
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c);
        c = tmp;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], email[256], phone[256];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255[^,],%255[^,],%255[^,]", name, email, phone) == 3) {
                add_contact(name, email, phone);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", name) == 1) find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}