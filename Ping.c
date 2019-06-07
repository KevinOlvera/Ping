#include "comnet.c"

int main(int argc, char *argv[])
{
    char hostname[16];
    char ipstring_dest[17];
    unsigned char My_MAC[6], My_IP[4], ownNetMask[4];
    unsigned char Hw_Addr_target[6], IPDestino[4];
    int ds, ifindex = 0, pID;
    struct ifreq Interfaz;
    unsigned char trama_icmp[98];
    int icmp_data = sizeof(trama_icmp) - 14;
    int succesPacket = 0, sec = 0;
    int lostPacket = 0;
    char select;

    ds = Socket_Raw();
  
    ifindex = getData(ds, My_MAC, My_IP, ownNetMask);

    printf(" Ingresa la IP a escanear\n -> ");
    scanf("%s", hostname);

    if (Hostname_to_IP(hostname, ipstring_dest) == -1)
    {
        printf("Direccion Invalida :(");
        exit(1);
    }

    printf(" PING a %s\n", ipstring_dest);
    IP_String_to_Array(ipstring_dest, IPDestino);
    sleep(1);

    if (Host_is_in_Network(My_IP, ownNetMask, IPDestino))
        ARP(My_MAC, My_IP, IPDestino, Hw_Addr_target, &ifindex);
    else
    {
        unsigned char Gateway_IP[4];
        Gateway_Address(Gateway_IP);
        ARP(My_MAC, My_IP, Gateway_IP, Hw_Addr_target, &ifindex);
    }

    Eth_header(trama_icmp, Hw_Addr_target, My_MAC, ETHTYPE_ICMP, (int)sizeof(trama_icmp));
    pID = getpid() * 2;
    signal(SIGINT, inthand);
    usleep(300);

    while (!stop)
    {
        if (ICMP(trama_icmp, (int)sizeof(trama_icmp), ICMP_PROT, Hw_Addr_target, My_MAC, My_IP, IPDestino, sec + 1, pID, ds, ifindex))
            succesPacket++;
        sec++;
        sleep(1);
    }

    lostPacket = sec - succesPacket;
    printf(" \n--- %s ping ---\n", hostname);
    printf(" %d paquetes enviados - %d paquetes recibidos - %d paquetes perdidos\n", (sec), succesPacket, lostPacket);
    close(ds);
    usleep(100);
    return 0;
}