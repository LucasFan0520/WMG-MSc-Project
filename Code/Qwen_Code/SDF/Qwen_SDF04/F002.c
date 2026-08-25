// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* tag;
    char* message;
} Record;

Record* records = NULL;
size_t r_count = 0;
size_t r_cap = 0;

void cleanup() {
    for (size_t i = 0; i < r_count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
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
            char* tag = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* msg = ptr;
            
            int found = 0;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (r_count == r_cap) {
                    size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                    Record* temp = realloc(records, new_cap * sizeof(Record));
                    if (!temp) { cleanup(); free(line); return 1; }
                    records = temp;
                    r_cap = new_cap;
                }
                records[r_count].tag = strdup(tag);
                records[r_count].message = strdup(msg);
                if (!records[r_count].tag || !records[r_count].message) { cleanup(); free(line); return 1; }
                r_count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* tag1 = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* tag2 = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* newtag = ptr;
            
            char* msg1 = NULL;
            char* msg2 = NULL;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].tag, tag1) == 0) msg1 = records[i].message;
                if (strcmp(records[i].tag, tag2) == 0) msg2 = records[i].message;
            }
            
            if (msg1 && msg2) {
                size_t len1 = strlen(msg1);
                size_t len2 = strlen(msg2);
                size_t total = len1 + 1 + len2;
                char* combined = malloc(total + 1);
                if (!combined) { cleanup(); free(line); return 1; }
                snprintf(combined, total + 1, "%s %s", msg1, msg2);
                
                int found = 0;
                for (size_t i = 0; i < r_count; i++) {
                    if (strcmp(records[i].tag, newtag) == 0) {
                        found = 1;
                        free(records[i].message);
                        records[i].message = combined;
                        break;
                    }
                }
                if (!found) {
                    if (r_count == r_cap) {
                        size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                        Record* temp = realloc(records, new_cap * sizeof(Record));
                        if (!temp) { free(combined); cleanup(); free(line); return 1; }
                        records = temp;
                        r_cap = new_cap;
                    }
                    records[r_count].tag = strdup(newtag);
                    records[r_count].message = combined;
                    if (!records[r_count].tag) { free(combined); cleanup(); free(line); return 1; }
                    r_count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* tag = ptr;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    free(records[i].tag);
                    free(records[i].message);
                    for (size_t j = i; j < r_count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* tag = ptr;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    for (size_t j = 0; records[i].message[j]; j++) {
                        if (records[i].message[j] == ' ') putchar('_');
                        else putchar(records[i].message[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}