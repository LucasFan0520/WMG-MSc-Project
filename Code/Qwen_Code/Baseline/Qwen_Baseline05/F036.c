// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_DATA 1000

typedef struct {
    char name[MAX_NAME];
    char data[MAX_DATA];
} Record;

Record records[1000];
int rec_count = 0;

int find_rec(const char *name) {
    for (int i = 0; i < rec_count; i++) if (strcmp(records[i].name, name) == 0) return i;
    return -1;
}

void print_underscores(const char *s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] == ' ') putchar('_');
        else putchar(s[i]);
    }
    putchar('\n');
}

int main() {
    char cmd[20], name[MAX_NAME], input[MAX_DATA + 20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", name, input);
            char *colon = strchr(input, ':');
            if (colon) {
                *colon = '\0';
                int len = atoi(input);
                char *data = colon + 1;
                if ((int)strlen(data) == len) {
                    strcpy(records[rec_count].name, name);
                    strcpy(records[rec_count].data, data);
                    rec_count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                for (int i = idx; i < rec_count - 1; i++) records[i] = records[i + 1];
                rec_count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) print_underscores(records[idx].data);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rec_count; i++) printf("%s\n", records[i].name);
        }
    }
    return 0;
}