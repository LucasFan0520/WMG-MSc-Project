// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* key; char* value; } Record;

Record* records = NULL;
size_t r_count = 0, r_cap = 0;

void cleanup() {
    for (size_t i = 0; i < r_count; i++) {
        free(records[i].key);
        free(records[i].value);
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
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* key = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* value = ptr;

            if (r_count == r_cap) {
                size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                Record* temp = realloc(records, new_cap * sizeof(Record));
                if (!temp) { cleanup(); free(line); return 1; }
                records = temp; r_cap = new_cap;
            }
            records[r_count].key = strdup(key);
            records[r_count].value = strdup(value);
            if (!records[r_count].key || !records[r_count].value) { cleanup(); free(line); return 1; }
            r_count++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (size_t i = 0; i < r_count; ) {
                int dup = 0;
                for (size_t j = 0; j < i; j++) {
                    if (strcmp(records[i].key, records[j].key) == 0) {
                        dup = 1;
                        break;
                    }
                }
                if (dup) {
                    free(records[i].key);
                    free(records[i].value);
                    for (size_t j = i; j < r_count - 1; j++) records[j] = records[j + 1];
                    r_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; ) {
                if (strcmp(records[i].key, ptr) == 0) {
                    free(records[i].key);
                    free(records[i].value);
                    for (size_t j = i; j < r_count - 1; j++) records[j] = records[j + 1];
                    r_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].key, ptr) == 0) {
                    printf("%s\n", records[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s %s\n", records[i].key, records[i].value);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}