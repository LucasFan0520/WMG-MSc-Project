// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_EMAIL 100
#define MAX_PHONE 20

typedef struct {
    char name[MAX_NAME];
    char email[MAX_EMAIL];
    char phone[MAX_PHONE];
} Contact;

Contact contacts[1000];
int contact_count = 0;

int find_contact(const char *name) {
    for (int i = 0; i < contact_count; i++) {
        if (strcmp(contacts[i].name, name) == 0) return i;
    }
    return -1;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char line[500];
    while (fgets(line, sizeof(line), f)) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        
        if (name && email && phone) {
            strcpy(contacts[contact_count].name, name);
            strcpy(contacts[contact_count].email, email);
            strcpy(contacts[contact_count].phone, phone);
            contact_count++;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char cmd[20], input[500];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", input);
            char *name = strtok(input, ",");
            char *email = strtok(NULL, ",");
            char *phone = strtok(NULL, ",");
            
            if (name && email && phone) {
                int idx = find_contact(name);
                if (idx == -1) {
                    strcpy(contacts[contact_count].name, name);
                    strcpy(contacts[contact_count].email, email);
                    strcpy(contacts[contact_count].phone, phone);
                    contact_count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", input);
            int idx = find_contact(input);
            if (idx != -1) {
                for (int i = idx; i < contact_count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                contact_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", input);
            int idx = find_contact(input);
            if (idx != -1) {
                printf("%s,%s,%s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < contact_count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }
    
    return 0;
}