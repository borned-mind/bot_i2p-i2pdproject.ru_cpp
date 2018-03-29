#include "socket.hpp"

namespace Sockets{
Socket::Socket(void){
//... ... ... ... ...
this->status_sock=not_inited;
//... ... ... ... ...
}Socket::Socket(int domain,int type,int protocol){
this->self_socket=this->init_socket(domain,type,protocol);
}Socket::Socket(const char * host, int port,type_sock type, int domain , int protocol ){
   if( type == tcp )
    this->self_socket=this->init_socket_tcp(domain,protocol);
   else if ( type == udp )
    this->self_socket=this->init_socket_udp(domain,protocol);
   this->connect_to(host,port);
}



Socket::~Socket(void){
   this->close_self_sock();
}
bool Socket::close_connect(int socket){
   close(socket);
}
void Socket::close_self_sock(void){
   if(!this->status_sock) throw(not_exist_sock);
   close(this->self_socket);
   this->status_sock = not_inited;
}void Socket::close_socket(int socket){
   close(socket);
}


bool Socket::connect_to(const char * host,int port){
   if(this->status_sock > 1) throw(socket_used_for_other);
   struct sockaddr_in serv_addr;
   struct hostent *server;
   server = gethostbyname(host);
   if(!server) throw(not_founded_host);
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = this->sock_family;
   bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
   serv_addr.sin_port = htons(port);
   if (connect(this->self_socket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) throw(connecting_refused);
   this->status_sock = connected;
   return true;
}bool Socket::binding(const char * host,int port,int maxlisten){
   if(this->status_sock > 1) throw(socket_used_for_other);
   struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = this->sock_family;
    serv_addr.sin_addr.s_addr = inet_addr(host);
    serv_addr.sin_port = htons(port);
    if (bind(this->self_socket, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
             throw(bad_banding);
    listen( this->self_socket, maxlisten );
    this->status_sock=server;
    return true;
}

int Socket::getsockopt_(int socket,int level, int optname,void *optval, socklen_t *optlen){
return getsockopt(socket, level, optname,optval, optlen);
}int Socket::setsockopt_(int socket,int level, int optname,const void *optval, socklen_t optlen){
int returns ;
if(returns = setsockopt (socket, level, optname, optval,optlen) < 0) throw(setsockopt_err);
return returns;
}
void Socket::set_sock(int socket){
  this->self_socket = socket;
}
int Socket::init_socket(int domain, int type, int protocol){
   #ifdef WIN32
   WSADATA wsaData;
   DWORD dwError;
   // Initialize Winsock
   if ( (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) )
      throw( (int)WSAStartup_Failed );
   #endif
  this->sock_family=domain;
  int sockfd = socket(domain, type, protocol);
  if(sockfd == -1) throw(init_sock_err);
  this->status_sock = inited;
  return sockfd;
}int Socket::init_socket_udp(int domain, int protocol){ // simply recursive
   return this->init_socket(domain,SOCK_DGRAM,protocol);
}int Socket::init_socket_tcp(int domain, int protocol){ // simply recursive
   return this->init_socket(domain,SOCK_STREAM,protocol);
}int Socket::init_socket_icmp(
      int domain,
      int type,
      unsigned int /*__attribute__((aligned(8)))*/ type_icmp , 
      unsigned short echo_id ,
      unsigned short echo_sequence 

){
this->self_icmp_packet.icmph.type=type_icmp;
this->self_icmp_packet.icmph.un.echo.id=echo_id;
this->self_icmp_packet.icmph.un.echo.sequence=echo_sequence;
this->self_icmp_packet.icmp_buf=new char[SIZEBUFFER];
memcpy(this->self_icmp_packet.icmp_buf, &this->self_icmp_packet.icmph, sizeof(this->self_icmp_packet.icmph));
return this->init_socket(domain,type, IPPROTO_ICMP);
}int Socket::init_socket_raw(
        int domain,
	bool ownHeader,
	const char * source_ip,
	const char * dest_ip,
	int source_port,
	int dest_port,
	unsigned char ihl,
	unsigned char ver,
	unsigned char tos,
	unsigned char TTL,
	unsigned char protocol
){
if(!ownHeader){
  return this->init_socket(domain,SOCK_RAW, IPPROTO_RAW);
}
else{
 int s = this->init_socket(domain,SOCK_RAW, IPPROTO_RAW);
 if( s == -1 ) throw(init_sock_err);
 this->self_raw_packet.raw_buf = new char[SIZEBUFFER];
 this->self_raw_packet.ip = (struct ipheader *) this->self_raw_packet.raw_buf;
 this->self_raw_packet.header = (struct raw_header *) (this->self_raw_packet.raw_buf + sizeof(struct ipheader));
 this->self_raw_packet.socket=&s;
 this->self_raw_packet.ip->iph_ihl = ihl;
 this->self_raw_packet.ip->iph_ver = ver;
 this->self_raw_packet.ip->iph_tos = tos; 
 this->self_raw_packet.ip->iph_len = sizeof(struct ipheader) + sizeof(struct raw_header);
 //this->self_raw_packet->ipheader->iph_ident = htons(54321); // <<<< --- ident?
 this->self_raw_packet.ip->iph_ttl = TTL;
 this->self_raw_packet.ip->iph_protocol = protocol; 
 this->self_raw_packet.ip->iph_sourceip = inet_addr(source_ip);
 this->self_raw_packet.ip->iph_destip = inet_addr(dest_ip);
 this->self_raw_packet.header->srcport = htons(source_port);
 this->self_raw_packet.header->destport = htons(dest_port);
 this->self_raw_packet.header->len = htons(sizeof(struct raw_header));
 //this->self_raw_packet->ipheader->iph_chksum = csum((unsigned short *)this->raw_buf, sizeof(struct ipheader) + sizeof(struct raw_header));
 this->setsockopt_(s,IPPROTO_IP, IP_HDRINCL, (const void *)1, sizeof(int));
 return s;
}

}

int Socket::get_descriptor_of_self_socket(void){
   return this->self_socket;
}

Socket::user_struct Socket::accepting(void){

Socket::user_struct returns;
struct sockaddr_in cli_addr;
int newsock;
socklen_t  clientlen  = sizeof(cli_addr);
newsock = accept(this->self_socket,
				  (struct sockaddr *) &cli_addr,
								 &clientlen);
if(newsock < 0) throw(bad_accept);
returns.socket=newsock;
returns.cli_addr=cli_addr;
return returns;
}Socket::user_struct Socket::accepting(int socket){

Socket::user_struct returns;
struct sockaddr_in cli_addr;
int newsock;
socklen_t  clientlen  = sizeof(cli_addr);
newsock = accept(socket,
				  (struct sockaddr *) &cli_addr,
								 &clientlen);
if(newsock < 0) throw(bad_accept);
returns.socket=newsock;
returns.cli_addr=cli_addr;
return returns;
}

bool Socket::writeBytes(const char * message, size_t n , int signal, struct sockaddr * to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){
if(send(this->self_socket,message,n,signal) == -1) throw(bad_write);
}else{
if(sendto(this->self_socket, message,n, signal,
               to, this->sockaddr_len) == -1) throw ( bad_write );
}

}

bool Socket::write(std::string message,int signal,struct sockaddr *to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){
if(send(this->self_socket,message.c_str(),message.size(),signal) == -1) throw(bad_write);
}else{
if(sendto(this->self_socket, message.c_str(),message.size(), signal,
               to, this->sockaddr_len) == -1) throw ( bad_write );
}

return true;
}bool Socket::write(std::wstring message,int signal,struct sockaddr *to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){
if(send(this->self_socket,message.c_str(),message.size(),signal) == -1) throw(bad_write);
}else{
if(sendto(this->self_socket, message.c_str(),message.size(), signal,
               to, this->sockaddr_len) == -1) throw ( bad_write );
}

return true;
}bool Socket::write(const char * message,int signal,struct sockaddr *to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){
 if(send(this->self_socket,message,strlen(message),signal) == -1) throw(bad_write);
}else{

if(sendto(this->self_socket, message, strlen(message), signal,
               to, this->sockaddr_len) == -1) throw ( bad_write );
}

return true;
}bool Socket::write(const unsigned char * message,int signal,struct sockaddr *to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){

if(send(this->self_socket,message,strlen_unsigned(message),signal) == -1) throw(bad_write);
}else{
if(sendto(this->self_socket, message, strlen_unsigned(message), signal,
               to, this->sockaddr_len) == -1) throw ( bad_write );
}
return true;
}

bool Socket::writeTo(int socket,std::string message,int signal,struct sockaddr *to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){
 if(send(socket,message.c_str(),message.size(),signal) == -1) throw(bad_write);
}else{
if(sendto(socket, message.c_str(),message.size(), signal,
               to, this->sockaddr_len) == -1) throw ( bad_write );
}

return true;
}bool Socket::writeTo(int socket,std::wstring message,int signal,struct sockaddr *to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){
 if(send(socket,message.c_str(),message.size(),signal) == -1) throw(bad_write);
}else{
if(sendto(socket, message.c_str(),message.size(), signal,
               to, sizeof(struct sockaddr_in)) == -1) throw ( bad_write );
}

return true;
}bool Socket::writeTo(int socket,const char * message,int signal,struct sockaddr *to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){
 if(send(socket,message,strlen(message),signal) == -1) throw(bad_write);
}else{
if(sendto(socket, message, strlen(message), signal,
               to, sizeof(struct sockaddr_in)) == -1) throw ( bad_write );
}


return true;
}bool Socket::writeTo(int socket,const unsigned char * message,int signal,struct sockaddr *to){
if(!this->status_sock) throw(not_exist_sock);
if(to == 0){
 if(send(socket,message,strlen_unsigned(message),signal) == -1) throw(bad_write);
}else{
if(sendto(socket, message, strlen_unsigned(message), signal,
               to, sizeof(struct sockaddr_in)) == -1) throw ( bad_write );
}
return true;
}

char * Socket::Read_from(int socket,unsigned long long sizebuf){
   char * buffer = new char[sizebuf];
   //memset(buffer,0,sizebuf);
   #ifdef WIN32
   if((recv(socket, buffer, sizebuf-1, 0)) <=0 ) throw(bad_read);
   #else
   if(read(socket,buffer,sizebuf-1) == -1) throw(bad_read);
   #endif
   return buffer;
}Socket::udp_packet Socket::Read_UDP(unsigned long long sizebuf,int flags){
  Socket::udp_packet returns;
  returns.fromlen=sizeof(returns.from);
  char * buffer = new char[sizebuf];

 if( recvfrom(this->self_socket, buffer, sizebuf-1, flags,
                 (struct sockaddr *)&returns.from, &returns.fromlen) == -1 )throw(bad_read);
  returns.message=buffer;
  return returns;
}Socket::udp_packet Socket::Read_Other(unsigned long long sizebuf,int flags){
  Socket::udp_packet returns;
  returns.fromlen=sizeof(returns.from);
  char * buffer = new char[sizebuf];

 if( recvfrom(this->self_socket, buffer, sizebuf-1, flags,
                 (struct sockaddr *)&returns.from, &returns.fromlen) == -1 )throw(bad_read);
  returns.message=buffer;
  return returns;
}char * Socket::Read(unsigned long long sizebuf){
char * buffer = new char[sizebuf];
//memset(buffer,0,sizebuf);
#ifdef WIN32
if((recv(this->self_socket, buffer, sizebuf-1, 0)) <=0 ) throw(bad_read);
#else
if(read(this->self_socket,buffer,sizebuf-1) == -1) throw(bad_read);
#endif
return buffer;
}



int Socket::shutdown_sock(int how){
shutdown(this->self_socket,how);
}int Socket::shutdown_sock(int socket,int how){
shutdown(socket,how);
}

}//namespace

namespace Dark{

Socks5Proxy::Socks5Proxy(const char * host,int port,
const char * proxy_host,const int proxy_port){

try{
set_sock(init_socket_tcp());
}

catch(Sockets::for_throws err){
this->error=true;
try{
	close_self_sock(); // close self sock.
}catch(Sockets::for_throws err){
	this->error=true;
}


}// try 13:0

if(!this->error){


	try{
		connect_to(proxy_host,proxy_port); // conecting to proxy
	}catch( Sockets::for_throws err ){
	try{
		close_self_sock(); // close self sock.
	}catch(Sockets::for_throws err){
		this->error=true;// if error with closing fd
	}
		this->error=true;// if error with connect
	} 

}// if not error



if( !SocksConnect(host,port) ){

try{
 close_self_sock(); // close self sock.
}catch(Sockets::for_throws err){}//try{...}catch{}
 this->error=true;

}//if


}


bool Socks5Proxy::ReConnectToDark(void){
try{
shutdown_sock(0); // close read in fd.
shutdown_sock(1); // close write in fd.
close_self_sock(); // close self sock.
}catch(Sockets::for_throws err){
return false;
} 
if(!BackHost || !BackPort) return false;
Socks5Proxy(BackHost,BackPort);
return SocksConnect();
}

bool Socks5Proxy::SocksConnect(void){
if(!BackHost || !BackPort) return false;
return SocksConnect(BackHost,BackPort);
}bool Socks5Proxy::SocksConnect(const char * host,const int port){
//set streaming
byte * bytes = new byte[4];
bytes[0] = 0x05;
bytes[1] = 0x01;
bytes[2] = 0x00;
try{
writeBytes((const char*)bytes,3);
}catch(Sockets::for_throws err){
delete [] bytes;
return false;
}



char * server_answer = Read(4);
if( server_answer[1] != 0  ){
delete [] bytes;
return false; // error
}
if( !BackHost || !BackPort){
// not connected, we have to set
BackHost = strdup(host);
BackPort = port;
}


delete  [] server_answer;


char  hostLen = (char)strlen(host);
char* LastRequst = new char[4 + 1 + hostLen + 2];
short HPort = htons(port);

bytes[3]=3;

memcpy(LastRequst, bytes, 4);                // 5, 1, 0, 3
memcpy(LastRequst + 4, &hostLen, 1);        // Domain Length 1 byte
memcpy(LastRequst + 5, host, hostLen);    // Domain 
memcpy(LastRequst + 5 + hostLen, &HPort, 2); // Port

writeBytes((const char*)LastRequst,4 + 1 + hostLen + 2);

delete [] LastRequst;


server_answer=Read(10);
if(server_answer[1] != 0){
delete [] bytes;
return false;
}

delete  [] server_answer;
delete [] bytes;

this->connected=true;
return true;
}

}


#undef SIZEBUFFER
