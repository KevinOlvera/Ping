#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <asm/types.h>
#include <ifaddrs.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/rtnetlink.h>
#include <signal.h>

#define TTLPATH "/proc/sys/net/ipv4/ip_default_ttl"
#define ETHTYPE_ARP "\x08\x06"
#define ETHTYPE_ICMP "\x08\x00"
#define BROADHWADDR "\xff\xff\xff\xff\xff\xff"
#define ICMP_PROT 1
#define PINGTOTLEN 84
#define BUFFER_SIZE 4096
#define YEL "\x1B[33m"
#define RESET "\x1B[0m"

volatile sig_atomic_t stop;
void usage(char **);
int Hostname_to_IP(char *, char *);
const char *Hostname_from_IP(char *);
int IPstringToArray(char *, unsigned char *);
int Socket_Raw(void);
int obtenerDatos(int, struct ifreq *, unsigned char *, unsigned char *, unsigned char *);
int Default_Interfaz(char *);
int Host_is_in_Network(unsigned char *, unsigned char *, unsigned char *);
int Gateway_Address(unsigned char *);
void Eth_header(unsigned char *, unsigned char *, unsigned char *, unsigned char *, int);
void ARP_Header(unsigned char *, unsigned char *, unsigned char *);
void IP_Header(unsigned char *, unsigned int, unsigned char *, unsigned char *, int);
unsigned short checksum(unsigned char *, int);
unsigned int getTTL(void);
void ARP(unsigned char *, unsigned char *, unsigned char *, unsigned char *, int *);
int sendARP(unsigned char *, unsigned char *, unsigned char *, unsigned char *, int, int *, int);
void rcvARP(unsigned char *, unsigned char *, unsigned char *, int, int *, int, unsigned char *);
int ICMP(unsigned char *, int, int, unsigned char *, unsigned char *, unsigned char *, unsigned char *, int, int, int, int);
int rcvICMP(unsigned char *, int, unsigned char *, unsigned char *, unsigned char *, unsigned char *, int, unsigned short, int, int, struct timeval *);
int filterICMPreply(unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned char *, int, unsigned short, int, struct timeval *);
void enviarTrama(unsigned char *, int, int, int);
int recibeTrama(int, unsigned char *, int);
int filterARPreply(unsigned char *, int, unsigned char *, unsigned char *, int);
void imprimeTrama(unsigned char *, int);
void inthand(int);