// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

Contact *contacts = NULL;
int count = 0;

void parse_and_add(char *str) {
    str[strcspn(str, "\r\n")] = 0;
    char *c1 = strchr(str, ',');
    if (!c1) return;
    char *c2 = strchr(c1 + 1, ',');
    if (!c2) return;
    *c1 = '\0';
    *c2 = '\0';
    char *name = str;
    char *email = c1 + 1;
    char *phone = c2 + 1;
    if (strlen(name) == 0 || strlen(email) == 0 || strlen(phone) == 0) {
        *c1 = ',';
        *c2 = ',';
        return;
    }
    contacts = realloc(contacts, (count + 1) * sizeof(Contact));
    contacts[count].name = strdup(name);
    contacts[count].email = strdup(email);
    contacts[count].phone = strdup(phone);
    count++;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char fline[1024];
        while (fgets(fline, sizeof(fline), fp)) {
            parse_and_add(fline);
        }
        fclose(fp);
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            parse_and_add(args);
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, args) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    for (int j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    contacts = realloc(contacts, count * sizeof(Contact));
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, args) == 0) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
