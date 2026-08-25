// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Packet {
    char *id;
    int header_len;
    int payload_len;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *packets = NULL;

Packet *find_packet(const char *id) {
    Packet *p = packets;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void packet_add(const char *id, int hl, int pl, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hl < 0 || pl < 0) return;
    if ((int)strlen(header) != hl || (int)strlen(payload) != pl) return;
    Packet *p = malloc(sizeof(Packet));
    if (!p) return;
    p->id = safe_strdup(id);
    p->header_len = hl;
    p->payload_len = pl;
    p->header = safe_strdup(header);
    p->payload = safe_strdup(payload);
    p->next = packets;
    packets = p;
}

void packet_drop(const char *id) {
    Packet *prev = NULL, *cur = packets;
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            if (prev) prev->next = cur->next;
            else packets = cur->next;
            free(cur->id);
            free(cur->header);
            free(cur->payload);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void packet_header(const char *id) {
    Packet *p = find_packet(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", p->header);
}

void packet_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", p->payload);
}

void packet_stats(void) {
    long total = 0;
    Packet *p = packets;
    while (p) {
        total += p->payload_len;
        p = p->next;
    }
    printf("%ld\n", total);
}

void free_all(void) {
    Packet *p = packets;
    while (p) {
        Packet *next = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512], arg3[512];
        int n = sscanf(line, "%31s %511s %511s %511s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;

        if (strcmp(cmd, "PACKET") == 0 && n >= 4) {
            int hl = atoi(arg2);
            int pl = atoi(arg3);
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            // now p points to header, after that payload
            char header[MAX_LINE], payload[MAX_LINE];
            if (sscanf(p, "%s %[^\n]", header, payload) != 2) continue;
            packet_add(arg1, hl, pl, header, payload);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 2) {
            packet_drop(arg1);
        } else if (strcmp(cmd, "HEADER") == 0 && n >= 2) {
            packet_header(arg1);
        } else if (strcmp(cmd, "PAYLOAD") == 0 && n >= 2) {
            packet_payload(arg1);
        } else if (strcmp(cmd, "STATS") == 0) {
            packet_stats();
        }
    }
    free_all();
    return 0;
}