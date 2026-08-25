// F016.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

int main(int argc, char **argv) {
    CSVContact *contacts = NULL;
    int count = 0;
    int capacity = 0;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            while ((fread = getline(&fline, &flen, f)) != -1) {
                if (fread > 0 && fline[fread - 1] == '\n') fline[fread - 1] = '\0';
                char *name = fline;
                char *comma1 = strchr(name, ',');
                if (!comma1) continue;
                *comma1 = '\0';
                char *email = comma1 + 1;
                char *comma2 = strchr(email, ',');
                if (!comma2) continue;
                *comma2 = '\0';
                char *phone = comma2 + 1;
                
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    CSVContact *tmp = realloc(contacts, new_cap * sizeof(CSVContact));
                    if (!tmp) return 1;
                    contacts = tmp;
                    capacity = new_cap;
                }
                contacts[count].name = strdup(name);
                contacts[count].email = strdup(email);
                contacts[count].phone = strdup(phone);
                count++;
            }
            free(fline);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *comma1 = strchr(name, ',');
            if (!comma1) continue;
            *comma1 = '\0';
            char *email = comma1 + 1;
            char *comma2 = strchr(email, ',');
            if (!comma2) continue;
            *comma2 = '\0';
            char *phone = comma2 + 1;
            
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                CSVContact *tmp = realloc(contacts, new_cap * sizeof(CSVContact));
                if (!tmp) return 1;
                contacts = tmp;
                capacity = new_cap;
            }
            contacts[count].name = strdup(name);
            contacts[count].email = strdup(email);
            contacts[count].phone = strdup(phone);
            count++;
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
                printf("%s,%s,%s\n", contacts[found].name, contacts[found].email, contacts[found].phone);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
