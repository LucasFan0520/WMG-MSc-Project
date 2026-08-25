// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *email, *phone, *note; } Contact;
Contact *contacts = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i=0; i<n; i++) if (!strcmp(contacts[i].name, name)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256], c[256], rest[1700];
        b[0] = c[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) sscanf(line, "%*s %s %s %s", a, b, c);
        else if (!strcmp(cmd, "NOTE")) sscanf(line, "%*s %s %[^\n]", a, rest);
        else if (!strcmp(cmd, "MERGE")) sscanf(line, "%*s %s %s", a, b);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "ADD")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap*2 : 4; contacts = realloc(contacts, cap*sizeof(Contact)); }
                contacts[n].name = strdup(a); contacts[n].email = strdup(b); contacts[n].phone = strdup(c); contacts[n].note = strdup(""); n++;
            }
        } else if (!strcmp(cmd, "NOTE")) {
            int i = find(a);
            if (i >= 0) { free(contacts[i].note); contacts[i].note = strdup(rest); }
        } else if (!strcmp(cmd, "MERGE")) {
            int iK = find(a), iR = find(b);
            if (iK >= 0 && iR >= 0) {
                char *new_note = malloc(strlen(contacts[iK].note) + strlen(contacts[iR].note) + 2);
                sprintf(new_note, "%s %s", contacts[iK].note, contacts[iR].note);
                free(contacts[iK].note); contacts[iK].note = new_note;
                free(contacts[iR].name); free(contacts[iR].email); free(contacts[iR].phone); free(contacts[iR].note);
                for (int j=iR; j<n-1; j++) contacts[j] = contacts[j+1]; n--;
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int i = find(a);
            if (i >= 0) {
                free(contacts[i].name); free(contacts[i].email); free(contacts[i].phone); free(contacts[i].note);
                for (int j=i; j<n-1; j++) contacts[j] = contacts[j+1]; n--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int i = find(a);
            if (i >= 0) printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
        } else if (!strcmp(cmd, "LIST")) {
            for (int i=0; i<n; i++) printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
        }
    }
    return 0;
}