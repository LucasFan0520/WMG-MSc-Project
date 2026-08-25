// filename: F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    int header_len;
    int payload_len;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

static Packet *packets = NULL;

static void cleanup(void) {
    while (packets) {
        Packet *tmp = packets;
        packets = packets->next;
        free(tmp->id);
        free(tmp->header);
        free(tmp->payload);
        free(tmp);
    }
}

static Packet *find_packet(const char *id) {
    for (Packet *p = packets; p; p = p->next)
        if (strcmp(p->id, id) == 0)
            return p;
    return NULL;
}

static void packet_add(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    if (!p) return;
    p->id = strdup(id);
    p->header = strdup(header);
    p->payload = strdup(payload);
    if (!p->id || !p->header || !p->payload) {
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        return;
    }
    p->header_len = hlen;
    p->payload_len = plen;
    p->next = packets;
    packets = p;
}

static void drop_packet(const char *id) {
    Packet *prev = NULL;
    for (Packet *p = packets; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else packets = p->next;
            free(p->id);
            free(p->header);
            free(p->payload);
            free(p);
            return;
        }
        prev = p;
    }
}

static void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

static void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

static void stats(void) {
    long total = 0;
    for (Packet *p = packets; p; p = p->next)
        total += p->payload_len;
    printf("%ld\n", total);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "PACKET") == 0) {
            if (*arg1 && *arg2 && *arg3) {
                char *arg4 = arg3;
                while (*arg4 && *arg4 != ' ') ++arg4;
                if (*arg4) *arg4++ = '\0';
                char *arg5 = arg4;
                // arg2 is headerLen, arg3 is payloadLen
                int hlen = atoi(arg2);
                int plen = atoi(arg3);
                // arg4 is header, arg5 is payload (rest)
                packet_add(arg1, hlen, plen, arg4, arg5);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (*arg1) drop_packet(arg1);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (*arg1) print_header(arg1);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (*arg1) print_payload(arg1);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }
    free(line);
    return 0;
}