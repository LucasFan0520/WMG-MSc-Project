// F015.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

Contact *contacts = NULL;
int count = 0;
int capacity = 0;

int find_contact(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void add_c(const char *name, const char *email, const char *phone) {
    int idx = find_contact(name);
    if (idx != -1) {
        free(contacts[idx].email);
        free(contacts[idx].phone);
        contacts[idx].email = strdup(email);
        contacts[idx].phone = strdup(phone);
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        contacts = realloc(contacts, capacity * sizeof(Contact));
    }
    contacts[count].name = strdup(name);
    contacts[count].email = strdup(email);
    contacts[count].phone = strdup(phone);
    contacts[count].note = strdup("");
    count++;
}

void note_c(const char *name, const char *text) {
    int idx = find_contact(name);
    if (idx != -1) {
        free(contacts[idx].note);
        contacts[idx].note = strdup(text);
    }
}

void merge_c(const char *keep, const char *remove) {
    int idxK = find_contact(keep);
    int idxR = find_contact(remove);
    if (idxK != -1 && idxR != -1) {
        char *nK = contacts[idxK].note;
        char *nR = contacts[idxR].note;
        size_t new_len = strlen(nK) + strlen(nR) + 2;
        char *new_note = malloc(new_len);
        if (strlen(nK) > 0 && strlen(nR) > 0) {
            sprintf(new_note, "%s %s", nK, nR);
        } else if (strlen(nK) > 0) {
            strcpy(new_note, nK);
        } else {
            strcpy(new_note, nR);
        }
        free(contacts[idxK].note);
        contacts[idxK].note = new_note;
        
        free(contacts[idxR].name);
        free(contacts[idxR].email);
        free(contacts[idxR].phone);
        free(contacts[idxR].note);
        for (int i = idxR; i < count - 1; i++) {
            contacts[i] = contacts[i + 1];
        }
        count--;
    }
}

void delete_c(const char *name) {
    int idx = find_contact(name);
    if (idx != -1) {
        free(contacts[idx].name);
        free(contacts[idx].email);
        free(contacts[idx].phone);
        free(contacts[idx].note);
        for (int i = idx; i < count - 1; i++) {
            contacts[i] = contacts[i + 1];
        }
        count--;
    }
}

void print_contact(int idx) {
    printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone, contacts[idx].note);
}

void find_c(const char *name) {
    int idx = find_contact(name);
    if (idx != -1) {
        print_contact(idx);
    }
}

void list_c(void) {
    for (int i = 0; i < count; i++) {
        print_contact(i);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *email = strtok(NULL, " ");
            char *phone = strtok(NULL, " ");
            if (name && email && phone) add_c(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (name && text) note_c(name, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = strtok(NULL, " ");
            char *remove = strtok(NULL, " ");
            if (keep && remove) merge_c(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_c(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_c(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_c();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}
