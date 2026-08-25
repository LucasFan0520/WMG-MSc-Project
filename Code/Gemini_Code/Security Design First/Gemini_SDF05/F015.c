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

int main(void) {
    Contact *contacts = NULL;
    int count = 0;
    int capacity = 0;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space1 = strchr(name, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *email = space1 + 1;
            while (*email == ' ') email++;
            char *space2 = strchr(email, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *phone = space2 + 1;
            while (*phone == ' ') phone++;
            
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Contact *tmp = realloc(contacts, new_cap * sizeof(Contact));
                if (!tmp) return 1;
                contacts = tmp;
                capacity = new_cap;
            }
            char *nn = strdup(name);
            char *ne = strdup(email);
            char *np = strdup(phone);
            if (!nn || !ne || !np) return 1;
            contacts[count].name = nn;
            contacts[count].email = ne;
            contacts[count].phone = np;
            contacts[count].note = strdup("");
            count++;
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            char *text = "";
            if (space) {
                *space = '\0';
                text = space + 1;
                while (*text == ' ') text++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *nt = strdup(text);
                if (!nt) return 1;
                free(contacts[found].note);
                contacts[found].note = nt;
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *keep = p;
            char *space = strchr(keep, ' ');
            if (!space) continue;
            *space = '\0';
            char *remove = space + 1;
            while (*remove == ' ') remove++;
            
            int fk = -1, fr = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, keep) == 0) fk = i;
                if (strcmp(contacts[i].name, remove) == 0) fr = i;
            }
            if (fk != -1 && fr != -1) {
                size_t l1 = strlen(contacts[fk].note);
                size_t l2 = strlen(contacts[fr].note);
                char *nnote = malloc(l1 + l2 + 2);
                if (!nnote) return 1;
                strcpy(nnote, contacts[fk].note);
                if (l1 > 0 && l2 > 0) strcat(nnote, " ");
                strcat(nnote, contacts[fr].note);
                free(contacts[fk].note);
                contacts[fk].note = nnote;
                
                free(contacts[fr].name);
                free(contacts[fr].email);
                free(contacts[fr].phone);
                free(contacts[fr].note);
                for (int i = fr; i < count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(contacts[found].name);
                free(contacts[found].email);
                free(contacts[found].phone);
                free(contacts[found].note);
                for (int i = found; i < count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s %s %s %s\n", contacts[found].name, contacts[found].email, contacts[found].phone, contacts[found].note);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
            }
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
