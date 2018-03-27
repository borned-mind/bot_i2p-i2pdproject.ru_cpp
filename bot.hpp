#include"commands.hpp"

#include<map>
#include<functional>
#include<future>


constexpr long long secondstimeout=1;
constexpr char secsthrdelete=15;

enum class ProxyType{
	none, http, socks5
};

struct ProxyArgs{
	std::string host_prox;
	unsigned int port_proxy;
	ProxyType type;
	unsigned int port_serv;
	ProxyArgs(std::string host_prox="localhost", unsigned int port_proxy=4447, ProxyType type=ProxyType::socks5, unsigned int port_serv=5222):
			host_prox(host_prox), port_proxy(port_proxy), type(type), port_serv(port_serv){} 
};


class Bot : public gloox::MessageHandler, public Commands, public gloox::ConnectionListener{
private:
	
	typedef void (Bot::*pMethodForMsg)(gloox::Client & client, const gloox::Message & stanza);
	gloox::Client * client;
	std::map<std::string, pMethodForMsg> fMessMap;


private:
	bool needCloseAfter, wasConnected;

	void  onConnect (void) noexcept override;
	void  onDisconnect (gloox::ConnectionError e) override{
		std::cout << "Connection is lost"  << std::endl;
	}
	bool  onTLSConnect (const gloox::CertInfo &info)noexcept override{
		std::cout << "Tls connect" << std::endl;
	}
private:

	void threadMsgHandle(const gloox::Message & stanza, gloox::MessageSession* session = 0) noexcept;

	std::vector< std::thread > threads;
	std::thread * delThread;
	void deleteThread(void){

		std::this_thread::sleep_for( std::chrono::seconds( secsthrdelete ) );
		std::cout << "Delete thread" << std::endl;
		for( auto it = threads.begin(); it!=threads.end(); it++ ){
			it->join();
		}
		threads.resize(0);
		deleteThread();
	}
private:
	void handleMessage(const gloox::Message & stanza, gloox::MessageSession* session = 0) noexcept override;
	void InstallDB(void);
	inline void set_proxy(ProxyArgs & proxArg, const std::string & server_addr){
			gloox::ConnectionTCPClient * tcpclient = new gloox::ConnectionTCPClient(client->logInstance(), proxArg.host_prox, proxArg.port_proxy);
			switch(proxArg.type){
				case ProxyType::socks5:
					client->setConnectionImpl(new gloox::ConnectionSOCKS5Proxy( client, tcpclient,
							client->logInstance(), server_addr, proxArg.port_serv) );
					break;
				case ProxyType::http:
					client->setConnectionImpl(new gloox::ConnectionHTTPProxy(client, tcpclient,
                                                       client->logInstance(), server_addr, proxArg.port_serv )  );
					break;
				case ProxyType::none:
				default:
					throw( std::runtime_error( "You setuse proxy, but in proxy type you set none") );
			}
			std::cout << "Proxy was set" << std::endl;
	}
public:
	Bot(std::string & serv_addr, bool UseProxy=true, bool dissconectAfter=true, ProxyArgs proxArg=ProxyArgs{});
	Bot(gloox::JID jid, std::string password, bool UseProxy=false, ProxyArgs proxArg=ProxyArgs{});	
	~Bot(void);
	void connect_to(gloox::JID jid, std::string password, bool UseProxy, ProxyArgs proxArg);

};
