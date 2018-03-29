#include"commands.hpp"
#include"bot.hpp"
#include"SimplyClassSocket.hpp"

constexpr const char add_server_table[] = "INSERT INTO servers(serv_addr) values(?)";
constexpr const char get_list_str[] = "select serv_addr from servers";
constexpr const char help_str[]="help -> help menu\n"
				 "get_list -> get list of servers\n"
				 "add @serv -> add server to list\n"
				  "ping -> ping of bot\n"
					"check @serv_addr @port\n" ;

constexpr const char ping_str[] = "PONG";

constexpr auto timeoutSec = std::chrono::seconds(120);


bool Commands::isXMPP(std::string & servaddr)noexcept{
	try{
		
		std::cout << "Connect to server" << std::endl;
		Bot j(servaddr);
		return true;
	}catch(...){
		return false;
	}
}


void Commands::Help(gloox::Client & client, const gloox::Message & stanza) noexcept{
			std::cout << "Help" << std::endl;
			send_msg(std::ref(client), std::ref(stanza), help_str);		
}


void Commands::InitList(void){
    std::cout << "Init list" << std::endl;
    std::vector<database::Row> rows;
    if(!db->sql(get_list_str, rows)) 
        throw( std::runtime_error( db->error() ) );

  
   for(auto row : rows) 
	for(auto column : row){

		servers.push_back( column.second.get() );
		std::cout << "Add to servers list: " << column.first << " " << column.second << std::endl;
	}
  
}

void Commands::Get_list(gloox::Client & client, const gloox::Message & stanza){
	std::cout << "Get list" << std::endl;
	if ( servers.size() ){
		std::ostringstream str;
		for( auto serv : servers ){
			str << serv << "\n";
		} 
		send_msg( std::ref(client), std::ref(stanza), str.str() );
	}else{
		send_msg( std::ref(client), std::ref(stanza), "List is empty" );
	}
} 

void Commands::Ping(gloox::Client & client, const gloox::Message & stanza){
	send_msg(std::ref(client), std::ref(stanza), ping_str);
}

void Commands::Check(gloox::Client & client, const gloox::Message & stanza){
try{
	std::istringstream stream(stanza.body());
	std::vector<std::string> args;
	std::string tmp;
	while( std::getline( stream, tmp, ' ') )
		args.push_back(tmp);
	if(args.size() < 3 || args.size()  > 3){
		send_msg(std::ref(client), std::ref(stanza), "check @serv_addr @port");
	}else{
		
			Dark::Socks5Proxy host(args[1].c_str(), atoi( args[2].c_str() ) );
			if( !host.connected_succesfully() ){
				send_msg(std::ref(client), std::ref(stanza), "Can't connect");
			}else{
				send_msg(std::ref(client), std::ref(stanza), "Connected");
			}
		
	}
}catch(Sockets::for_throws thr){
			send_msg(std::ref(client), std::ref(stanza), "ERROR with connecting");
		}
}

void Commands::Add_list(gloox::Client & client, const gloox::Message & stanza){
    std::cout << "add serv" << std::endl;
    std::istringstream stream( stanza.body() );
    std::vector<std::string> args;
    std::string tmp;
    while( std::getline( stream, tmp, ' ') ){
		args.push_back(tmp);
    }



    if( args.size() < 2 || args.size() > 2) 
	send_msg(std::ref(client), std::ref(stanza), "add @serv_addr");
    else if( serv_exists(args[1]) )
	send_msg(std::ref(client), std::ref(stanza), "Serv already added.");	
    else{

	 //auto methd = std::bind(&Commands::isXMPP, this, std:: _Placeholder<1>() ) ;
	 auto methd = std::bind(&Commands::isXMPP, this, args[1] );
	 auto isXMPPRes = std::async( std::launch::async, methd );
	 if(  isXMPPRes.wait_for(timeoutSec) == std::future_status::timeout  )
			send_msg(std::ref(client), std::ref(stanza), "Timeout.");
	else if ( isXMPPRes.get() == false )
			send_msg(std::ref(client), std::ref(stanza), "It is not XMPP?.");
	else if(!db->sql( add_server_table, args[1] ))
        		throw( std::runtime_error( db->error() ) );
    	else{
		std::cout << "added " << args[1] << std::endl;
  		servers.push_back( args[1] );
   		send_msg( std::ref(client), std::ref(stanza), "Added" );
    	     }
   }
}
