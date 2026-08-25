// F040.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    int payload_len;
} Packet;

Packet *cache = NULL;
int count = 0;
int capacity = 0;

int find_packet(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(cache[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

void add_packet(const char *id, int h_len, int p_len, const char *header, const char *payload) {
    if ((int)strlen(header) != h_len || (int)strlen(payload) != p_len) return;
    if (find_packet(id) != -1) return;
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        cache = realloc(cache, capacity * sizeof(Packet));
    }
    cache[count].id = strdup(id);
    cache[count].header = strdup(header);
    cache[count].payload = strdup(payload);
    cache[count].payload_len = p_len;
    count++;
}

void drop_packet(const char *id) {
    int idx = find_packet(id);
    if (idx != -1) {
        free(cache[idx].id);
        free(cache[idx].header);
        free(cache[idx].payload);
        for (int i = idx; i < count - 1; i++) {
            cache[i] = cache[i + 1];
        }
        count--;
    }
}

void print_header(const char *id) {
    int idx = find_packet(id);
    if (idx != -1) {
        printf("%s\n", cache[idx].header);
    }
}

void print_payload(const char *id) {
    int idx = find_packet(id);
    if (idx != -1) {
        printf("%s\n", cache[idx].payload);
    }
}

void print_stats(void) {
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += cache[i].payload_len;
    }
    printf("%d\n", total);
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = strtok(NULL, " ");
            char *hl_s = strtok(NULL, " ");
            char *pl_s = strtok(NULL, " ");
            char *header = strtok(NULL, " ");
            char *payload = strtok(NULL, "");
            if (id && hl_s && pl_s && header && payload) {
                add_packet(id, atoi(hl_s), atoi(pl_s), header, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = strtok(NULL, "");
            if (id) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = strtok(NULL, "");
            if (id) print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = strtok(NULL, "");
            if (id) print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            print_stats();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
