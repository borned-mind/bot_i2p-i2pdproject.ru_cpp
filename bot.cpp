#include"bot.hpp"

constexpr const char help_com[] = "help";
constexpr const char get_list_com[] = "get_list";
constexpr const char add_com[] = "add";


constexpr const char servers_table_install[] = "create table servers(id INTEGER PRIMARY KEY AUTOINCREMENT , serv_addr varchar)";

void Bot::threadMsgHandle(const gloox::Message & stanza, gloox::MessageSession* session) noexcept{
			const gloox::Message & nstanza= std::move(stanza);
			std::cout << "Handle" << std::endl;
			std::stringstream stream(nstanza.body());
			std::cout << "Stream was created" << std::endl;
			std::string body;
			stream>>body;

			try{
				fMessMap.at(body);
				( this->*(fMessMap[body]) )(*client, std::ref(nstanza) ) ;
			}catch(...){
				( this->*(fMessMap[help_com]) )(*client, std::ref(nstanza) ) ;
			}
}

void Bot::handleMessage(const gloox::Message & stanza, gloox::MessageSession* session ) noexcept{
	//TODO: copy 
	threads.push_back(std::move( std::thread(&Bot::threadMsgHandle, this, std::ref(stanza), session) )  );
	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
}

Bot::~Bot(void){
	delete client;
	//sqlite3_close(db);
}

void Bot::InstallDB(void){
	db->sql(servers_table_install);
	//sql_exec(servers_table_install);
}

Bot::Bot(std::string & serv_addr, bool UseProxy, bool dissconectAfter, ProxyArgs proxArg){
	client = new gloox::Client{serv_addr};
  	client->registerConnectionListener( this );

	if(UseProxy)
		set_proxy(proxArg,  serv_addr);

	needCloseAfter=dissconectAfter;
	client->connect(true);
	std::cout << "Connected "  << std::endl;

	if( !wasConnected  ){
		std::cout << "error" << std::endl;
		throw (std::runtime_error("Can't connect") ) ;
	}
	if(dissconectAfter)
		client->disconnect();
}

Bot::Bot(gloox::JID jid, std::string password, bool UseProxy, ProxyArgs proxArg){
		std::cout << "Set MessMap" << std::endl;
		fMessMap[help_com]=&Bot::Help;
		fMessMap[get_list_com]=&Bot::Get_list;
		fMessMap[add_com]=&Bot::Add_list;


		if( !*(db = new database("servers.db")) )
			throw( std::runtime_error( "Can't open server database" ) );

		if( FILE * tmp = fopen("installed.c" , "rb")  )
			fclose(tmp);
		else{
			tmp = fopen("installed.c", "wb");
			InstallDB();
			fwrite( "1", 1, 1,  tmp );
			fclose(tmp);
		}
		InitList();
		delThread = new std::thread( &Bot::deleteThread, this );
		

		

		connect_to( std::move(jid), std::move(password), UseProxy, proxArg  );
}

void Bot::connect_to(gloox::JID jid, std::string password, bool UseProxy, ProxyArgs proxArg){
		client = new gloox::Client{jid, password};
		client->registerMessageHandler(this);
  		client->registerConnectionListener( this );


		if(UseProxy)
			set_proxy(proxArg,  jid.server());
		/*
			client->setConnectionImpl( proxArg.type == ProxyType::socks5 ? 
					new gloox::ConnectionSOCKS5Proxy( client, tcpclient,
							client->logInstance(), jid.server(), proxArg.port_serv) 
					: new gloox::ConnectionHTTPProxy(client, tcpclient, client->logInstance(), jid.server(), proxArg.port_serv )  );
		*/
	client->connect();
	if( !wasConnected  ){
		std::cout << "error" << std::endl;
		throw (std::runtime_error("Can't connect") ) ;
	}
}

//Not work?!
void Bot::onConnect(void) noexcept{
		std::cout << "Connection estabilished" << std::endl;
		wasConnected=true;
		if(needCloseAfter)
			client->disconnect();
}



