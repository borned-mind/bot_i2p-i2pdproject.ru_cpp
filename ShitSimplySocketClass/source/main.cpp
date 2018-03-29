#include "socket.hpp"
#define RAWTEST


int main(int argcount,char**arguments){
static Sockets::Socket * sock;
#ifdef SIMPLYHTTPCONNECTTEST
sock = new Sockets::Socket("google.com",80);
sock->write("GET /\r\n");
printf("%s\n",sock->Read());
delete sock;
sock = new Sockets::Socket();
sock->set_sock(sock->init_socket_tcp());
try{
sock->connect_to("localhost",80);
sock->write("GET /\r\n");
printf("%s\n",sock->Read());
}catch(Sockets::for_throws err){
  if ( err == 1 ) printf("You are not set sock");
}
delete sock;
#endif


#ifdef UDPTEST
sock = new Sockets::Socket();
sock->set_sock(sock->init_socket_udp());
sock->binding("0.0.0.0",15055);
Sockets::Socket::udp_packet tmp;
while(1){
try{
   tmp = sock->Read_UDP();
   char ipv4[INET_ADDRSTRLEN];
   inet_ntop(AF_INET, &tmp.from.sin_addr, ipv4, INET_ADDRSTRLEN);
   sock->write(tmp.message,0,(struct sockaddr*)&tmp.from);
   printf("%s:%d write -> %s\n",ipv4,ntohs(tmp.from.sin_port),tmp.message);
   delete tmp.message;
}catch(Sockets::for_throws err){
}
}
#endif
#ifdef TCPTEST
sock = new Sockets::Socket();
sock->set_sock(sock->init_socket_tcp());
sock->binding("0.0.0.0",15055);
Sockets::Socket::user_struct tmp;
while(1){
try{
tmp = sock->accepting();
char ipv4[INET_ADDRSTRLEN];

inet_ntop(AF_INET, &tmp.cli_addr.sin_addr, ipv4, INET_ADDRSTRLEN);

sock->writeTo(tmp.socket,"Test Test how you are see this text?\r\n");
printf("%s:%d write -> %s\n",ipv4,ntohs(tmp.cli_addr.sin_port),sock->Read_from(tmp.socket));
close(tmp.socket);
}catch(Sockets::for_throws err){
}
}
#endif

#ifdef ICMPTEST
struct icmphdr icmp_hdr;
struct sockaddr_in addr;
char packetdata[sizeof(icmp_hdr) + 3];
icmp_hdr.type = ICMP_ECHO;
icmp_hdr.un.echo.id = 666;
icmp_hdr.un.echo.sequence = 1;

memcpy(packetdata, &icmp_hdr, sizeof(icmp_hdr));
memcpy(packetdata + sizeof(icmp_hdr), "667", 3);

memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(0x7F000001);


sock = new Sockets::Socket();
sock->set_sock(sock->init_socket_icmp());
Sockets::Socket::icmp_packet tmp;
sock->write(packetdata,0,(struct sockaddr*)&addr);
#endif

#ifdef RAWTEST

#endif


}
