// F040.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    size_t payload_len;
} PacketCache;

int main(void) {
    PacketCache *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            char *hlen_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                hlen_str = p;
            }
            while (*p && *p != ' ') p++;
            char *plen_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                plen_str = p;
            }
            while (*p && *p != ' ') p++;
            char *header = p;
            while (*p && *p != ' ') p++;
            char *payload = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                payload = p;
            }
            int hlen = atoi(hlen_str);
            int plen = atoi(plen_str);
            if (hlen >= 0 && plen >= 0) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    PacketCache *tmp = realloc(arr, capacity * sizeof(PacketCache));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].id = strdup(id);
                arr[count].header = strdup(header);
                arr[count].payload = strdup(payload);
                arr[count].payload_len = (size_t)plen;
                if (!arr[count].id || !arr[count].header || !arr[count].payload) exit(1);
                count++;
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].header);
                    free(arr[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s\n", arr[i].header);
                    break;
                }
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            while (*id == ' ') id++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s\n", arr[i].payload);
                    break;
                }
            }
        } else if (strcmp(line, "STATS") == 0) {
            unsigned long long total_bytes = 0;
            for (size_t i = 0; i < count; i++) {
                total_bytes += arr[i].payload_len;
            }
            printf("%llu\n", total_bytes);
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].header);
        free(arr[i].payload);
    }
    free(arr);
    free(line);
    return 0;
}
