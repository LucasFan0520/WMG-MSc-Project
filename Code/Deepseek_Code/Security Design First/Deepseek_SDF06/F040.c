// F040.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    size_t header_len;
    size_t payload_len;
    char *header;
    char *payload;
} Packet;

static Packet *packets = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_packet(const char *id) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(packets[i].id, id) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_packet(const char *id, size_t hlen, size_t plen, const char *header, const char *payload) {
    if (find_packet(id) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Packet *new = realloc(packets, capacity * sizeof(Packet));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        packets = new;
    }
    packets[count].id = strdup(id);
    if (!packets[count].id) { fprintf(stderr, "Memory error\n"); exit(1); }
    // Validate lengths match strings
    if (strlen(header) != hlen || strlen(payload) != plen) {
        free(packets[count].id);
        return;
    }
    packets[count].header_len = hlen;
    packets[count].payload_len = plen;
    packets[count].header = strdup(header);
    packets[count].payload = strdup(payload);
    if (!packets[count].header || !packets[count].payload) {
        free(packets[count].id);
        free(packets[count].header);
        free(packets[count].payload);
        return;
    }
    count++;
}

static void drop_packet(const char *id) {
    ssize_t idx = find_packet(id);
    if (idx == -1) return;
    free(packets[idx].id);
    free(packets[idx].header);
    free(packets[idx].payload);
    for (size_t i = idx; i < count - 1; i++)
        packets[i] = packets[i + 1];
    count--;
}

static void print_header(const char *id) {
    ssize_t idx = find_packet(id);
    if (idx == -1) return;
    printf("%s\n", packets[idx].header);
}

static void print_payload(const char *id) {
    ssize_t idx = find_packet(id);
    if (idx == -1) return;
    printf("%s\n", packets[idx].payload);
}

static void stats(void) {
    size_t total = 0;
    for (size_t i = 0; i < count; i++)
        total += packets[i].payload_len;
    printf("%zu\n", total);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(packets[i].id);
        free(packets[i].header);
        free(packets[i].payload);
    }
    free(packets);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], id[256], header[8192], payload[8192];
    size_t hlen, plen;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(line, "%*s %255s %zu %zu %8191[^\n]", id, &hlen, &plen, header) == 4) {
                // payload is after header; we need to separate header and payload based on lengths
                // Since sscanf reads the rest into header, we have to split manually.
                // The line format: PACKET id headerLength payloadLength header payload
                // We can find the header part and payload part by positions.
                // Use a different approach: parse from the line after id and lengths.
                // Let's reconstruct: use sscanf to read id, hlen, plen, then the remaining string.
                char *p = line;
                // skip command
                while (*p && *p != ' ') p++;
                while (*p && *p == ' ') p++;
                // id
                char id_buf[256];
                sscanf(p, "%255s", id_buf);
                p += strlen(id_buf);
                while (*p && *p == ' ') p++;
                // hlen
                size_t hl;
                sscanf(p, "%zu", &hl);
                while (*p && *p != ' ') p++;
                while (*p && *p == ' ') p++;
                // plen
                size_t pl;
                sscanf(p, "%zu", &pl);
                while (*p && *p != ' ') p++;
                while (*p && *p == ' ') p++;
                // now p points to header string
                char *header_start = p;
                // header length is hl
                if (strlen(header_start) < hl) break;
                char header_buf[8192];
                strncpy(header_buf, header_start, hl);
                header_buf[hl] = '\0';
                char *payload_start = header_start + hl;
                if (strlen(payload_start) < pl) break;
                char payload_buf[8192];
                strncpy(payload_buf, payload_start, pl);
                payload_buf[pl] = '\0';
                add_packet(id_buf, hl, pl, header_buf, payload_buf);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }

    free(line);
    free_all();
    return 0;
}