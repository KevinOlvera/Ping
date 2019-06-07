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
unsigned int Metric;
unsigned int MTU;

volatile sig_atomic_t stop;
int Hostname_to_IP(char *hostname, char *ip);
const char *Hostname_from_IP(char *ip);
int IP_String_to_Array(char *ipstring, unsigned char *iparray);
int Socket_Raw();
int getData(int sd, unsigned char *my_MAC, unsigned char *my_IP, unsigned char *NETMASK);
int Host_is_in_Network(unsigned char *miIP, unsigned char *mascara, unsigned char *host);
int Gateway_Address(unsigned char *Gateway_IP_array);
void Eth_header(unsigned char *trama, unsigned char *destHwAddr, unsigned char *sourceHwAddr, unsigned char *ethType, int trama_len);
void ARP_Header(unsigned char *trama, unsigned char *sourceIPAddr, unsigned char *targetIPAddr);
void IP_Header(unsigned char *trama, unsigned int protocol, unsigned char *sourceIPAddr, unsigned char *targetIPAddr, int count);
void ICMP_Header(unsigned char *trama, unsigned short seqNumber, int ID);
unsigned short checksum(unsigned char *buff, int bufflen);
unsigned int getTTL();
void ARP(unsigned char *sourceHwAddr, unsigned char *sourceIPAddr, unsigned char *targetIPAddr, unsigned char *Hw_Addr_target, int *ifindex);
int sendARP(unsigned char *trama_env, unsigned char *sourceHwAddr, unsigned char *sourceIPAddr, unsigned char *targetIPAddr, int trama_len, int *ifindex, int ds);
void rcvARP(unsigned char *trama_rcv, unsigned char *sourceHwAddr, unsigned char *targetIPAddr, int trama_len, int *ifindex, int ds, unsigned char *Hw_Addr_target);
int ICMP(unsigned char *trama_icmp, int trama_len, int protocolNumber, unsigned char *destHwAddr, unsigned char *sourceHwAddr, unsigned char *sourceIPAddr, unsigned char *targetIPAddr, int sqNumber, int pID, int ds, int index);
int rcvICMP(unsigned char *trama_rcv, int trama_len, unsigned char *destHwAddr, unsigned char *sourceHwAddr, unsigned char *targetIPAddr, unsigned char *sourceIPAddr, int pID, unsigned short sqNumber, int ds, int index, struct timeval *tval_now);
int filterICMPreply(unsigned char *trama, unsigned char *destHwAddr, unsigned char *sourceHwAddr, unsigned char *targetIPAddr, unsigned char *sourceIPAddr, int pID, unsigned short seqNumber, int tam_rcv_from, struct timeval *tval_now);
void enviarTrama(unsigned char *trama_enviar, int tramalen, int ds, int index);
int recibeTrama(int ds, unsigned char *trama, int trama_len);
int filterARPreply(unsigned char *trama, int trama_len, unsigned char *destHwAddr, unsigned char *sourceIPAddr, int trama_rcv_from_len);
void imprimeTrama(unsigned char *, int);
void inthand(int signum);