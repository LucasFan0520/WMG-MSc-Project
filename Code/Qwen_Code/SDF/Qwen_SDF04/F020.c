// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* title;
    char** body;
    size_t b_count;
    size_t b_cap;
} Record;

Record* records = NULL;
size_t r_count = 0;
size_t r_cap = 0;

void cleanup() {
    for (size_t i = 0; i < r_count; i++) {
        for (size_t j = 0; j < records[i].b_count; j++) {
            free(records[i].body[j]);
        }
        free(records[i].body);
        free(records[i].title);
    }
    free(records);
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    
    FILE* f = fopen(argv[1], "r");
    if (!f) return 1;
    
    char* line = NULL;
    size_t len = 0;
    int in_record = 0;
    Record* curr = NULL;
    
    while (getline(&line, &len, f) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        
        if (strcmp(line, "--") == 0) {
            if (in_record && curr) {
                curr = NULL;
                in_record = 0;
            } else {
                if (r_count == r_cap) {
                    size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                    Record* temp = realloc(records, new_cap * sizeof(Record));
                    if (!temp) { free(line); fclose(f); return 1; }
                    records = temp;
                    r_cap = new_cap;
                }
                curr = &records[r_count];
                curr->body = NULL;
                curr->b_count = 0;
                curr->b_cap = 0;
                in_record = 1;
            }
            continue;
        }
        
        if (in_record && curr) {
            if (!curr->title) {
                curr->title = strdup(line);
                if (!curr->title) { free(line); fclose(f); return 1; }
            } else {
                if (curr->b_count == curr->b_cap) {
                    size_t new_cap = curr->b_cap == 0 ? 4 : curr->b_cap * 2;
                    char** temp = realloc(curr->body, new_cap * sizeof(char*));
                    if (!temp) { free(line); fclose(f); return 1; }
                    curr->body = temp;
                    curr->b_cap = new_cap;
                }
                curr->body[curr->b_count] = strdup(line);
                if (!curr->body[curr->b_count]) { free(line); fclose(f); return 1; }
                curr->b_count++;
            }
        }
    }
    if (in_record && curr) {
        r_count++;
    }
    fclose(f);
    free(line);
    
    line = NULL;
    len = 0;
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
        
        if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].title, title) == 0) {
                    printf("%s\n", records[i].title);
                    for (size_t j = 0; j < records[i].b_count; j++) {
                        printf("%s\n", records[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].title, title) == 0) {
                    for (size_t j = 0; j < records[i].b_count; j++) {
                        free(records[i].body[j]);
                    }
                    free(records[i].body);
                    free(records[i].title);
                    for (size_t j = i; j < r_count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            
            if (r_count == r_cap) {
                size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                Record* temp = realloc(records, new_cap * sizeof(Record));
                if (!temp) { cleanup(); free(line); return 1; }
                records = temp;
                r_cap = new_cap;
            }
            records[r_count].title = strdup(title);
            records[r_count].body = NULL;
            records[r_count].b_count = 0;
            records[r_count].b_cap = 0;
            if (!records[r_count].title) { cleanup(); free(line); return 1; }
            
            while (getline(&line, &len, stdin) != -1) {
                if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
                if (strcmp(line, "--") == 0) break;
                
                if (records[r_count].b_count == records[r_count].b_cap) {
                    size_t new_cap = records[r_count].b_cap == 0 ? 4 : records[r_count].b_cap * 2;
                    char** temp = realloc(records[r_count].body, new_cap * sizeof(char*));
                    if (!temp) { cleanup(); free(line); return 1; }
                    records[r_count].body = temp;
                    records[r_count].b_cap = new_cap;
                }
                records[r_count].body[records[r_count].b_count] = strdup(line);
                if (!records[r_count].body[records[r_count].b_count]) { cleanup(); free(line); return 1; }
                records[r_count].b_count++;
            }
            r_count++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s\n", records[i].title);
                for (size_t j = 0; j < records[i].b_count; j++) {
                    printf("%s\n", records[i].body[j]);
                }
                printf("--\n");
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}