// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct C {
    char *name;
    char *email;
    char *phone;
    struct C *next;
} C;

C *head = NULL;

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        char *n = strtok(line, ",");
        char *e = strtok(NULL, ",");
        char *p = strtok(NULL, ",");
        if (n && e && p) {
            C *nc = malloc(sizeof(C));
            nc->name = strdup(n);
            nc->email = strdup(e);
            nc->phone = strdup(p);
            nc->next = head;
            head = nc;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char name[100];
    char email[100];
    char phone[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%[^,],%[^,],%s", name, email, phone);
            C *nc = malloc(sizeof(C));
            nc->name = strdup(name);
            nc->email = strdup(email);
            nc->phone = strdup(phone);
            nc->next = head;
            head = nc;
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            C *p = head;
            C *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    free(p->name);
                    free(p->email);
                    free(p->phone);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name);
            C *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                printf("%s,%s,%s\n", p->name, p->email, p->phone);
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (C *p = head; p; p = p->next) {
                printf("%s,%s,%s\n", p->name, p->email, p->phone);
            }
        }
    }
    return 0;
}