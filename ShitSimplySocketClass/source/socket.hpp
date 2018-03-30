#ifndef __cplusplus
 #error This code for C++
#endif

#pragma once
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#ifdef WIN32// || MINGW || __MINGW32__
   #include <windows.h>
   #include <winsock.h>
   #include <winsock2.h>
   #include <ws2tcpip.h>
   #define MSG_NOSIGNAL 0
   #pragma comment(lib, "ws2_32.lib")
#else
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <netinet/ip_icmp.h>
#endif
#include <string>
#include "somefunc.hpp"

#ifndef bzero
#define bzero(d,n) memset((d),0,(n))
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#endif

#define SIZEBUFFER 4096

namespace Sockets{
   enum for_throws{
      #ifdef WIN32
      WSAStartup_Failed,
      #endif
      init_sock_err,   not_exist_sock,
      not_founded_host, connecting_refused,
      bad_banding,bad_accept,bad_write,bad_read,socket_used_for_other,
      socket_not_inited,setsockopt_err,undefined_type_icmp
   };
      typedef unsigned char ubyte;
      typedef char byte;
   // The IP header's structure
   struct ipheader {
 	ubyte      iph_ihl:5, iph_ver:4;
 	ubyte      iph_tos;
 	unsigned short int iph_len;
 	unsigned short int iph_ident;
 	ubyte      iph_flag;
 	unsigned short int iph_offset;
 	ubyte      iph_ttl;
	ubyte      iph_protocol;
	unsigned short int iph_chksum;
 	unsigned int       iph_sourceip;
 	unsigned int       iph_destip;
	};
   struct raw_header {
	unsigned short int srcport;
 	unsigned short int destport;
 	unsigned short int len;
 	unsigned short int chksum;
	};
   struct icmp_struct{
    struct icmphdr icmph;
    char * icmp_buf;
   };
   struct raw_socket_struct{
    char * raw_buf;
    ipheader * ip;
    raw_header * header;
    int * socket;
   };
   class Socket{
   protected:
      int self_socket;
      int sock_family;
      raw_socket_struct self_raw_packet;
      icmp_struct self_icmp_packet;
   public:
      /*enum type_addr{
         ipv4,ipv6
      };*/
      enum type_sock{
         tcp,udp
      };enum status_of_socket{
         not_inited,inited,connected,server
      };
      struct user_struct{
         int socket;
         struct sockaddr_in cli_addr;
      };
      struct udp_packet{
         struct sockaddr_in from;
         socklen_t fromlen;
         char * message;
      };
/*
      enum type_of_icmp{
	raw,echo
      };
*/
      typedef udp_packet icmp_packet;
      typedef udp_packet raw_packet;

      



      const int sockaddr_len = sizeof(struct sockaddr_in);
      status_of_socket status_sock;
      Socket(void);
      Socket(int domain,int type,int protocol);
      Socket(const char * host,int port,type_sock type=tcp, int domain = AF_INET, int protocol = 0 );
      ~Socket(void);
      void close_self_sock(void);
      int get_descriptor_of_self_socket(void);

      int init_socket(int domain=AF_INET, int type=SOCK_STREAM, int protocol=0);
      int init_socket_udp(int domain=AF_INET,int protocol=0);
      int init_socket_tcp(int domain=AF_INET,int protocol=0);
      int init_socket_icmp(
      int domain=AF_INET,
      int type=SOCK_DGRAM,
      unsigned int /*__attribute__((aligned(8)))*/ type_icmp = ICMP_ECHO , 
      unsigned short echo_id = 1,
      unsigned short echo_sequence = 1
      );
      int init_socket_raw(
        int domain=AF_INET,
	bool ownHeader=true,
	const char * source_ip=0,
	const char * dest_ip=0,
	int source_port=0,
	int dest_port=0,
	unsigned char ihl = 5,
	unsigned char ver = 4,
	unsigned char tos = 16,
	unsigned char TTL = 128,
	unsigned char protocol = 17
	);


      int getsockopt_(int socket,int level, int optname,
                                      void *optval, socklen_t *optlen);
      int setsockopt_(int socket,int level, int optname,
                                      const void *optval, socklen_t optlen);
      void close_socket(int socket);
      user_struct accepting(int socket);
      user_struct accepting(void);

      bool writeBytes(const char * bytes,size_t n,int signal=MSG_NOSIGNAL,struct sockaddr * to=0);

      bool write( std::string message,int signal=MSG_NOSIGNAL,struct sockaddr *to=0);
      bool write( std::wstring message,int signal=MSG_NOSIGNAL,struct sockaddr *to=0);
      bool write( const char *  message,int signal=MSG_NOSIGNAL,struct sockaddr *to=0);
      bool write( const unsigned char *  message,int signal=MSG_NOSIGNAL,struct sockaddr *to=0);

      bool writeTo( int socket, std::string message,int signal=MSG_NOSIGNAL,struct sockaddr *to=0);
      bool writeTo( int socket, std::wstring message,int signal=MSG_NOSIGNAL,struct sockaddr *to=0);
      bool writeTo( int socket, const char *  message,int signal=MSG_NOSIGNAL,struct sockaddr *to=0);
      bool writeTo( int socket, const unsigned char *  message,int signal=MSG_NOSIGNAL,struct sockaddr *to=0);
      int shutdown_sock(int how);
      int shutdown_sock(int socket,int how);
      

      char * Read(unsigned long long sizebuf=1024);
      udp_packet Read_UDP(unsigned long long sizebuf=4096,int flags=0);
      udp_packet Read_Other(unsigned long long sizebuf=4096,int flags=0);
      char * Read_from(int socket,unsigned long long sizebuf=1024);
      void set_sock(int socket);
      virtual bool connect_to(const char * host,int port, struct timeval timeout={10,0});
      virtual bool binding(const char * host,int port,int maxlisten=100);
      virtual bool close_connect(int socket);

   };
}

typedef unsigned char __attribute__((mode(QI))) byte;

namespace Dark{

struct Socks{
byte version;
byte type;
byte host[2];
byte port[4];
byte idstring[10];
};




class Socks5Proxy : public Sockets::Socket{
private:
bool error;
protected:
bool connected=false;
char * BackHost=0;
int BackPort=0;
public:

bool isset_error(void){
	return error;
}bool connected_succesfully(void){
	return connected;
}bool get_error(void){
	return error;
}
Socks5Proxy(const char * proxy_host="127.0.0.1",const int proxy_port=4447);
bool ReConnectToDark(void);
bool SocksConnect(const char * host,const int port);
bool SocksConnect(void);
};

}


