// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    long payload_len;
} PacketRecord;

int main(void) {
    PacketRecord *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "PACKET") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *hlen_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *plen_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *header = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *payload = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long hlen = strtol(hlen_str, NULL, 10);
            long plen = strtol(plen_str, NULL, 10);

            if (hlen >= 0 && plen >= 0 && (size_t)hlen == strlen(header) && (size_t)plen == strlen(payload)) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    PacketRecord *new_items = realloc(items, capacity * sizeof(PacketRecord));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].id = strdup(id);
                items[count].header = strdup(header);
                items[count].payload = strdup(payload);
                items[count].payload_len = plen;
                if (!items[count].id || !items[count].header || !items[count].payload) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].header);
                    free(items[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s\n", items[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s\n", items[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long total = 0;
            for (size_t i = 0; i < count; i++) {
                total += items[i].payload_len;
            }
            printf("%ld\n", total);
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].id);
        free(items[i].header);
        free(items[i].payload);
    }
    free(items);
    free(line);
    return 0;
}
