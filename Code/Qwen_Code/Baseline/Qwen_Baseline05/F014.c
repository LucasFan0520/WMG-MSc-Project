// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_NOTE 1000
#define MAX_LABEL 100

typedef struct {
    char name[MAX_NAME];
    char note[MAX_NOTE];
    char labels[100][MAX_LABEL];
    int label_count;
} Item;

Item items[1000];
int item_count = 0;

int find_item(const char *name) {
    for (int i = 0; i < item_count; i++) {
        if (strcmp(items[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], note[MAX_NOTE], label[MAX_LABEL];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ITEM") == 0) {
            scanf("%s", name);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            
            strcpy(items[item_count].name, name);
            strcpy(items[item_count].note, note);
            items[item_count].label_count = 0;
            item_count++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            scanf("%s %s", name, label);
            int idx = find_item(name);
            if (idx != -1) {
                strcpy(items[idx].labels[items[idx].label_count], label);
                items[idx].label_count++;
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            scanf("%s %s", name, label);
            int idx = find_item(name);
            if (idx != -1) {
                for (int i = 0; i < items[idx].label_count; i++) {
                    if (strcmp(items[idx].labels[i], label) == 0) {
                        for (int j = i; j < items[idx].label_count - 1; j++) {
                            strcpy(items[idx].labels[j], items[idx].labels[j + 1]);
                        }
                        items[idx].label_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                for (int i = idx; i < item_count - 1; i++) {
                    items[i] = items[i + 1];
                }
                item_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                printf("%s %s\n", items[idx].name, items[idx].note);
                for (int i = 0; i < items[idx].label_count; i++) {
                    printf("  %s\n", items[idx].labels[i]);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < item_count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
                for (int j = 0; j < items[i].label_count; j++) {
                    printf("  %s\n", items[i].labels[j]);
                }
            }
        }
    }
    
    return 0;
}