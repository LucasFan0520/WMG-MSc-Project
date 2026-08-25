// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *data;
    int len;
} Buf;

Buf bufs[1000];
int bc = 0;

int find_name(char *name) {
    for (int i = 0; i < bc; i++) if (strcmp(bufs[i].name, name) == 0) return i;
    return -1;
}

int parse_hex(char *hex, unsigned char **out) {
    int l = strlen(hex);
    if (l % 2 != 0) return -1;
    *out = malloc(l / 2);
    for (int i = 0; i < l / 2; i++) {
        unsigned int b;
        if (sscanf(hex + i * 2, "%2x", &b) != 1) {
            free(*out);
            return -1;
        }
        (*out)[i] = b;
    }
    return l / 2;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "NEW") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *hex = rest + strlen(name);
            while (*hex == ' ') hex++;
            unsigned char *data;
            int len = parse_hex(hex, &data);
            if (len >= 0 && find_name(name) == -1) {
                bufs[bc].name = strdup(name);
                bufs[bc].data = data;
                bufs[bc].len = len;
                bc++;
            } else if (len >= 0) {
                free(data);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *hex = rest + strlen(name);
            while (*hex == ' ') hex++;
            unsigned char *data;
            int len = parse_hex(hex, &data);
            int idx = find_name(name);
            if (len >= 0 && idx != -1) {
                bufs[idx].data = realloc(bufs[idx].data, bufs[idx].len + len);
                memcpy(bufs[idx].data + bufs[idx].len, data, len);
                bufs[idx].len += len;
                free(data);
            } else if (len >= 0) {
                free(data);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[1000];
            int offset, length;
            sscanf(rest, "%s %d %d", name, &offset, &length);
            int idx = find_name(name);
            if (idx != -1 && offset >= 0 && length >= 0 && offset + length <= bufs[idx].len) {
                for (int i = 0; i < length; i++) {
                    printf("%02x", bufs[idx].data[offset + i]);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                free(bufs[idx].name);
                free(bufs[idx].data);
                for (int i = idx; i < bc - 1; i++) bufs[i] = bufs[i+1];
                bc--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bc; i++) {
                printf("%s %d\n", bufs[i].name, bufs[i].len);
            }
        }
    }
    for (int i = 0; i < bc; i++) {
        free(bufs[i].name);
        free(bufs[i].data);
    }
    return 0;
}