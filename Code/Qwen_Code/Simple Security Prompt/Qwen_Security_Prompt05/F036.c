// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} Record;

Record *records = NULL;
int num_records = 0;

int find_record(const char *name) {
    for (int i = 0; i < num_records; i++) {
        if (strcmp(records[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            char *payload = strstr(line, name);
            if (payload) {
                payload += strlen(name);
                if (*payload == ' ') payload++;
            } else {
                payload = "";
            }
            
            char *colon = strchr(payload, ':');
            if (colon) {
                int len = atoi(payload);
                char *data_start = colon + 1;
                if (strlen(data_start) == len) {
                    records = realloc(records, (num_records + 1) * sizeof(Record));
                    records[num_records].name = strdup(name);
                    records[num_records].data = strdup(data_start);
                    num_records++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_record(name);
            if (idx >= 0) {
                free(records[idx].name);
                free(records[idx].data);
                for (int i = idx; i < num_records - 1; i++) {
                    records[i] = records[i + 1];
                }
                num_records--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_record(name);
            if (idx >= 0) {
                for (int i = 0; records[idx].data[i]; i++) {
                    putchar(records[idx].data[i] == ' ' ? '_' : records[idx].data[i]);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < num_records; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }
    
    for (int i = 0; i < num_records; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    
    return 0;
}