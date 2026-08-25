// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main() {
    RLEString *strings = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "STORE") == 0) {
            char name[256], encoded[4096];
            if (sscanf(args, "%255s %4095s", name, encoded) == 2) {
                int valid = 1;
                char *p = encoded;
                while (*p != '\0') {
                    if (!isdigit(*p)) {
                        valid = 0;
                        break;
                    }
                    long val = 0;
                    while (*p != '\0' && isdigit(*p)) {
                        val = val * 10 + (*p - '0');
                        if (val > 100000) {
                            valid = 0;
                            break;
                        }
                        p++;
                    }
                    if (!valid || *p == '\0') {
                        valid = 0;
                        break;
                    }
                    p++;
                }
                if (valid) {
                    strings = realloc(strings, (count + 1) * sizeof(RLEString));
                    strings[count].name = strdup(name);
                    strings[count].encoded = strdup(encoded);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(strings[i].name, name) == 0) {
                        char *p = strings[i].encoded;
                        while (*p != '\0') {
                            int cnt = 0;
                            while (isdigit(*p)) {
                                cnt = cnt * 10 + (*p - '0');
                                p++;
                            }
                            char c = *p;
                            p++;
                            for (int j = 0; j < cnt; j++) {
                                if (c == ' ') printf("_");
                                else printf("%c", c);
                            }
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(strings[i].name, name) == 0) {
                        free(strings[i].name);
                        free(strings[i].encoded);
                        for (int j = i; j < count - 1; j++) {
                            strings[j] = strings[j + 1];
                        }
                        count--;
                        strings = realloc(strings, count * sizeof(RLEString));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", strings[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(strings[i].name);
        free(strings[i].encoded);
    }
    free(strings);
    return 0;
}
