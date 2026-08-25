// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_EMAIL 100
#define MAX_PHONE 20
#define MAX_NOTE 1000

typedef struct {
    char name[MAX_NAME];
    char email[MAX_EMAIL];
    char phone[MAX_PHONE];
    char note[MAX_NOTE];
} Contact;

Contact contacts[1000];
int contact_count = 0;

int find_contact(const char *name) {
    for (int i = 0; i < contact_count; i++) {
        if (strcmp(contacts[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], email[MAX_EMAIL], phone[MAX_PHONE], text[MAX_NOTE];
    char keep[MAX_NAME], remove[MAX_NAME];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s %s", name, email, phone);
            strcpy(contacts[contact_count].name, name);
            strcpy(contacts[contact_count].email, email);
            strcpy(contacts[contact_count].phone, phone);
            contacts[contact_count].note[0] = '\0';
            contact_count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            scanf("%s", name);
            fgets(text, MAX_NOTE, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            int idx = find_contact(name);
            if (idx != -1) {
                strcpy(contacts[idx].note, text);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%s %s", keep, remove);
            int idx_keep = find_contact(keep);
            int idx_remove = find_contact(remove);
            
            if (idx_keep != -1 && idx_remove != -1) {
                if (contacts[idx_remove].note[0] != '\0') {
                    if (contacts[idx_keep].note[0] != '\0') {
                        strcat(contacts[idx_keep].note, " ");
                    }
                    strcat(contacts[idx_keep].note, contacts[idx_remove].note);
                }
                
                for (int i = idx_remove; i < contact_count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                contact_count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_contact(name);
            if (idx != -1) {
                for (int i = idx; i < contact_count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                contact_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_contact(name);
            if (idx != -1) {
                printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email, 
                       contacts[idx].phone, contacts[idx].note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < contact_count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, 
                       contacts[i].phone, contacts[i].note);
            }
        }
    }
    
    return 0;
}