// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *email, *phone; } Contact;
Contact *contacts = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i=0; i<n; i++) if (!strcmp(contacts[i].name, name)) return i;
    return -1;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = 0;
        char *c1 = strchr(line, ','); if (!c1) continue;
        char *c2 = strchr(c1+1, ','); if (!c2) continue;
        *c1 = *c2 = 0;
        if (find(line) < 0) {
            if (n == cap) { cap = cap ? cap*2 : 4; contacts = realloc(contacts, cap*sizeof(Contact)); }
            contacts[n].name = strdup(line); contacts[n].email = strdup(c1+1); contacts[n].phone = strdup(c2+1); n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256], c[256];
        b[0] = c[0] = 0;
        sscanf(line, "%s %[^,],%[^,],%s", cmd, a, b, c);
        if (!strcmp(cmd, "ADD")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap*2 : 4; contacts = realloc(contacts, cap*sizeof(Contact)); }
                contacts[n].name = strdup(a); contacts[n].email = strdup(b); contacts[n].phone = strdup(c); n++;
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int i = find(a);
            if (i >= 0) {
                free(contacts[i].name); free(contacts[i].email); free(contacts[i].phone);
                for (int j=i; j<n-1; j++) contacts[j] = contacts[j+1]; n--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int i = find(a);
            if (i >= 0) printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i=0; i<n; i++) printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
        }
    }
    return 0;
}