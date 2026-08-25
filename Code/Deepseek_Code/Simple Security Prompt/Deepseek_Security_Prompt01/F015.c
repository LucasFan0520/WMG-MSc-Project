// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *email; char *phone; char *note; } Contact;
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
    contacts[cont_count].note = strdup("");
    cont_count++;
}

void note_contact(const char *name, const char *text) {
    int idx = find_contact(name);
    if (idx < 0) return;
    free(contacts[idx].note);
    contacts[idx].note = strdup(text);
}

void merge_contacts(const char *keep, const char *remove) {
    int ki = find_contact(keep), ri = find_contact(remove);
    if (ki < 0 || ri < 0) return;
    char *combined = malloc(strlen(contacts[ki].note) + strlen(contacts[ri].note) + 2);
    strcpy(combined, contacts[ki].note);
    if (strlen(contacts[ki].note) > 0 && strlen(contacts[ri].note) > 0)
        strcat(combined, " ");
    strcat(combined, contacts[ri].note);
    free(contacts[ki].note);
    contacts[ki].note = combined;
    free(contacts[ri].name);
    free(contacts[ri].email);
    free(contacts[ri].phone);
    free(contacts[ri].note);
    for (int i = ri; i < cont_count - 1; i++) contacts[i] = contacts[i + 1];
    cont_count--;
}

void delete_contact(const char *name) {
    int idx = find_contact(name);
    if (idx < 0) return;
    free(contacts[idx].name);
    free(contacts[idx].email);
    free(contacts[idx].phone);
    free(contacts[idx].note);
    for (int i = idx; i < cont_count - 1; i++) contacts[i] = contacts[i + 1];
    cont_count--;
}

void find_contact_print(const char *name) {
    int idx = find_contact(name);
    if (idx < 0) return;
    printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email,
           contacts[idx].phone, contacts[idx].note);
}

void list_contacts(void) {
    for (int i = 0; i < cont_count; i++)
        printf("%s %s %s %s\n", contacts[i].name, contacts[i].email,
               contacts[i].phone, contacts[i].note);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", arg1, arg2, arg3) == 3)
                add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg3) == 2)
                note_contact(arg1, arg3);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                merge_contacts(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_contact_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) list_contacts();
    }
    for (int i = 0; i < cont_count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}