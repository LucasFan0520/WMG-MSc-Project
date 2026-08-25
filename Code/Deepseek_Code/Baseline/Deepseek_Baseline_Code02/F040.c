// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    int headerLen;
    int payloadLen;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *head = NULL;

Packet *find_packet(const char *id) {
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen != (int)strlen(header) || plen != (int)strlen(payload)) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = malloc(strlen(id) + 1);
    strcpy(p->id, id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = malloc(hlen + 1);
    strcpy(p->header, header);
    p->payload = malloc(plen + 1);
    strcpy(p->payload, payload);
    p->next = head;
    head = p;
}

void drop_packet(const char *id) {
    Packet *prev = NULL;
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->id);
            free(p->header);
            free(p->payload);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->payload);
}

void stats(void) {
    int total = 0;
    Packet *p = head;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    Packet *p = head;
    while (p) {
        Packet *next = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            int hlen, plen;
            if (sscanf(p, "%d %d", &hlen, &plen) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            char *header = p;
            // skip hlen chars
            for (int i = 0; i < hlen; i++) p++;
            if (*p == 0) continue;
            *p = 0; p++;
            char *payload = p;
            if (strlen(header) != hlen || strlen(payload) != plen) continue;
            add_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            drop_packet(p);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            print_header(p);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            print_payload(p);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }
    free_all();
    return 0;
}