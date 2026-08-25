// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Con;

Con cons[1000];
int cc = 0;

int find_name(char *name) {
    for (int i = 0; i < cc; i++) if (strcmp(cons[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char name[1000], email[1000], phone[1000];
            sscanf(rest, "%s %s %s", name, email, phone);
            cons[cc].name = strdup(name);
            cons[cc].email = strdup(email);
            cons[cc].phone = strdup(phone);
            cons[cc].note = strdup("");
            cc++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *text = rest + strlen(name);
            while (*text == ' ') text++;
            int idx = find_name(name);
            if (idx != -1) {
                free(cons[idx].note);
                cons[idx].note = strdup(text);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[1000], rem[1000];
            sscanf(rest, "%s %s", keep, rem);
            int ik = find_name(keep), ir = find_name(rem);
            if (ik != -1 && ir != -1) {
                char *nn = malloc(strlen(cons[ik].note) + strlen(cons[ir].note) + 2);
                sprintf(nn, "%s %s", cons[ik].note, cons[ir].note);
                free(cons[ik].note);
                cons[ik].note = nn;
                free(cons[ir].name); free(cons[ir].email); free(cons[ir].phone); free(cons[ir].note);
                for (int i = ir; i < cc - 1; i++) cons[i] = cons[i+1];
                cc--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(cons[idx].name); free(cons[idx].email); free(cons[idx].phone); free(cons[idx].note);
                for (int i = idx; i < cc - 1; i++) cons[i] = cons[i+1];
                cc--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) printf("%s %s %s %s\n", cons[idx].name, cons[idx].email, cons[idx].phone, cons[idx].note);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < cc; i++) printf("%s %s %s %s\n", cons[i].name, cons[i].email, cons[i].phone, cons[i].note);
        }
    }
    for (int i = 0; i < cc; i++) {
        free(cons[i].name); free(cons[i].email); free(cons[i].phone); free(cons[i].note);
    }
    return 0;
}