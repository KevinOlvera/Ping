#include "Ping.c"

int main(int argc, char *argv[])
{
    char *hostname = argv[1];
    char ipstring_dest[17], InterfazName[10];
    unsigned char My_MAC[6], My_IP[4], ownNetMask[4];
    unsigned char Hw_Addr_target[6], IPDestino[4];
    int ds, ifindex = 0, pID;
    struct ifreq Interfaz;
    unsigned char trama_icmp[98];
    int icmp_data = sizeof(trama_icmp) - 14;
    int succesPacket = 0, sec = 0;
    float progress = 0.0;

    if (argc != 2)
    {
        printf("Ingresar parametros: %s [IP_Address/Hostname]\n", argv[0]);
        exit(1);
    }
    if (Hostname_to_IP(hostname, ipstring_dest) == -1)
    {
        printf("Direccion Invalida :(");
        exit(1);
    }

    ds = Socket_Raw();
   
    if (Default_Interfaz(InterfazName) == -1)
        exit(EXIT_FAILURE);
   
    strcpy(Interfaz.ifr_name, InterfazName);
    ifindex = obtenerDatos(ds, &Interfaz, My_MAC, My_IP, ownNetMask);
    printf("PING %s (%s) 56(%d) bytes de datos.\n", hostname, ipstring_dest, icmp_data);
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

    progress = ((float)succesPacket / (float)(sec)) * 100.0;
    progress = 100.0 - progress;
    printf("\n--- %s ping ---\n", hostname);
    printf("%d paquetes enviados, %d recibidos, %d%% paquetes perdidos\n", (sec), succesPacket, (int)progress);
    close(ds);
    usleep(100);
    return 0;
}