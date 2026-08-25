// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* item;
    int count;
} Inventory;

Inventory* inv = NULL;
size_t i_count = 0;
size_t i_cap = 0;

void cleanup() {
    for (size_t i = 0; i < i_count; i++) {
        free(inv[i].item);
    }
    free(inv);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
        
        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* item = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            int count = atoi(ptr);
            
            int found = 0;
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    found = 1;
                    inv[i].count += count;
                    break;
                }
            }
            if (!found) {
                if (i_count == i_cap) {
                    size_t new_cap = i_cap == 0 ? 4 : i_cap * 2;
                    Inventory* temp = realloc(inv, new_cap * sizeof(Inventory));
                    if (!temp) { cleanup(); free(line); return 1; }
                    inv = temp;
                    i_cap = new_cap;
                }
                inv[i_count].item = strdup(item);
                inv[i_count].count = count;
                if (!inv[i_count].item) { cleanup(); free(line); return 1; }
                i_count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* item = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            int count = atoi(ptr);
            
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    inv[i].count -= count;
                    if (inv[i].count < 0) inv[i].count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* item = ptr;
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    free(inv[i].item);
                    for (size_t j = i; j < i_count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    i_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* item = ptr;
            int count = 0;
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    count = inv[i].count;
                    break;
                }
            }
            printf("%d\n", count);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < i_count; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}