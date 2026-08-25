// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* key;
    char* value;
} Variable;

Variable* vars = NULL;
size_t v_count = 0;
size_t v_cap = 0;

void cleanup() {
    for (size_t i = 0; i < v_count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
}

void render_text(const char* text) {
    size_t len = strlen(text);
    char* out = malloc(len + 1);
    if (!out) return;
    size_t out_len = 0;
    
    for (size_t i = 0; i < len; ) {
        if (text[i] == '{') {
            size_t start = i + 1;
            size_t end = start;
            while (end < len && text[end] != '}') end++;
            
            if (end < len) {
                char* key = malloc(end - start + 1);
                if (key) {
                    memcpy(key, text + start, end - start);
                    key[end - start] = '\0';
                    
                    int found = 0;
                    for (size_t j = 0; j < v_count; j++) {
                        if (strcmp(vars[j].key, key) == 0) {
                            size_t val_len = strlen(vars[j].value);
                            char* temp = realloc(out, out_len + val_len + 1);
                            if (temp) {
                                out = temp;
                                memcpy(out + out_len, vars[j].value, val_len);
                                out_len += val_len;
                            }
                            found = 1;
                            break;
                        }
                    }
                    free(key);
                    
                    if (found) {
                        i = end + 1;
                        continue;
                    }
                }
            }
        }
        
        char* temp = realloc(out, out_len + 2);
        if (temp) {
            out = temp;
            out[out_len++] = text[i];
        }
        i++;
    }
    
    out[out_len] = '\0';
    printf("%s\n", out);
    free(out);
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
        
        if (strcmp(cmd, "SET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* value = ptr;
            
            int found = 0;
            for (size_t i = 0; i < v_count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    found = 1;
                    char* temp = strdup(value);
                    if (!temp) { cleanup(); free(line); return 1; }
                    free(vars[i].value);
                    vars[i].value = temp;
                    break;
                }
            }
            if (!found) {
                if (v_count == v_cap) {
                    size_t new_cap = v_cap == 0 ? 4 : v_cap * 2;
                    Variable* temp = realloc(vars, new_cap * sizeof(Variable));
                    if (!temp) { cleanup(); free(line); return 1; }
                    vars = temp;
                    v_cap = new_cap;
                }
                vars[v_count].key = strdup(key);
                vars[v_count].value = strdup(value);
                if (!vars[v_count].key || !vars[v_count].value) { cleanup(); free(line); return 1; }
                v_count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            for (size_t i = 0; i < v_count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (size_t j = i; j < v_count - 1; j++) {
                        vars[j] = vars[j + 1];
                    }
                    v_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            render_text(ptr);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < v_count; i++) {
                printf("%s=%s\n", vars[i].key, vars[i].value);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}