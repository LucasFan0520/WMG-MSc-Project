// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* key;
    char** values;
    size_t v_count;
    size_t v_cap;
} Index;

Index* indices = NULL;
size_t i_count = 0;
size_t i_cap = 0;

void cleanup() {
    for (size_t i = 0; i < i_count; i++) {
        for (size_t j = 0; j < indices[i].v_count; j++) {
            free(indices[i].values[j]);
        }
        free(indices[i].values);
        free(indices[i].key);
    }
    free(indices);
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
            char* key = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* value = ptr;
            
            Index* idx = NULL;
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(indices[i].key, key) == 0) {
                    idx = &indices[i];
                    break;
                }
            }
            if (!idx) {
                if (i_count == i_cap) {
                    size_t new_cap = i_cap == 0 ? 4 : i_cap * 2;
                    Index* temp = realloc(indices, new_cap * sizeof(Index));
                    if (!temp) { cleanup(); free(line); return 1; }
                    indices = temp;
                    i_cap = new_cap;
                }
                idx = &indices[i_count];
                idx->key = strdup(key);
                idx->values = NULL;
                idx->v_count = 0;
                idx->v_cap = 0;
                if (!idx->key) { cleanup(); free(line); return 1; }
                i_count++;
            }
            
            if (idx->v_count == idx->v_cap) {
                size_t new_cap = idx->v_cap == 0 ? 4 : idx->v_cap * 2;
                char** temp = realloc(idx->values, new_cap * sizeof(char*));
                if (!temp) { cleanup(); free(line); return 1; }
                idx->values = temp;
                idx->v_cap = new_cap;
            }
            idx->values[idx->v_count] = strdup(value);
            if (!idx->values[idx->v_count]) { cleanup(); free(line); return 1; }
            idx->v_count++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* value = ptr;
            
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(indices[i].key, key) == 0) {
                    for (size_t j = 0; j < indices[i].v_count; j++) {
                        if (strcmp(indices[i].values[j], value) == 0) {
                            free(indices[i].values[j]);
                            for (size_t k = j; k < indices[i].v_count - 1; k++) {
                                indices[i].values[k] = indices[i].values[k + 1];
                            }
                            indices[i].v_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(indices[i].key, key) == 0) {
                    for (size_t j = 0; j < indices[i].v_count; j++) {
                        free(indices[i].values[j]);
                    }
                    free(indices[i].values);
                    free(indices[i].key);
                    for (size_t j = i; j < i_count - 1; j++) {
                        indices[j] = indices[j + 1];
                    }
                    i_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(indices[i].key, key) == 0) {
                    for (size_t j = 0; j < indices[i].v_count; j++) {
                        printf("%s\n", indices[i].values[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < i_count; i++) {
                printf("%s\n", indices[i].key);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}