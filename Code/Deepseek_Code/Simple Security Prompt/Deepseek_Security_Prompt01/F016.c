// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *email; char *phone; } Contact;
Contact *contacts = NULL;
int cont_count = 0, cont_cap = 0;

int find_contact(const char *name) {
    for (int i = 0; i < cont_count; i++)
        if (strcmp(contacts[i].name, name) == 0) return i;
    return -1;
}

void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name) >= 0) return;
    if (cont_count == cont_cap) {
        cont_cap = cont_cap ? cont_cap * 2 : 4;
        contacts = realloc(contacts, sizeof(Contact) * cont_cap);
    }
    contacts[cont_count].name = strdup(name);
    contacts[cont_count].email = strdup(email);
    contacts[cont_count].phone = strdup(phone);
    cont_count++;
}

void delete_contact(const char *name) {
    int idx = find_contact(name);
    if (idx < 0) return;
    free(contacts[idx].name);
    free(contacts[idx].email);
    free(contacts[idx].phone);
    for (int i = idx; i < cont_count - 1; i++) contacts[i] = contacts[i + 1];
    cont_count--;
}

void find_contact_print(const char *name) {
    int idx = find_contact(name);
    if (idx < 0) return;
    printf("%s %s %s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone);
}

void report_contacts(void) {
    for (int i = 0; i < cont_count; i++)
        printf("%s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], email[256], phone[256];
        if (sscanf(line, "%255[^,],%255[^,],%255[^,\n]", name, email, phone) == 3)
            add_contact(name, email, phone);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], args[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(line, "%*s %255[^,],%255[^,],%255[^,\n]", name, email, phone) == 3)
                add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) report_contacts();
    }
    for (int i = 0; i < cont_count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}