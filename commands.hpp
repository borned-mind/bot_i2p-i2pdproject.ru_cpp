#pragma once
#include<gloox/connectiontcpclient.h>
#include<gloox/connectionsocks5proxy.h>
#include<gloox/client.h>
#include<gloox/messagehandler.h>
#include<gloox/connectionhttpproxy.h>
#include<gloox/message.h>
#include <gloox/connectionlistener.h>
#include<sqlite3.h> 
#include<iostream>
#include<vector>
#include<sstream>
#include<boost/regex.hpp>


#include"database.hpp"


//constexpr const char regex_dom_i2p[] = "^([a-z0-9]+(-[a-z0-9]+)*\\.)+i2p$";


class Commands{
	protected:
		database * db;
		std::vector<std::string> servers;
		//boost::regex reDomain;
	private:
		inline void send_msg(gloox::Client & client, const gloox::Message & stanza, std::string body) noexcept{
			gloox::Message msg( stanza.subtype(), stanza.from(), body );
			client.send( msg ) ;			
		}
		inline bool serv_exists(std::string serv) noexcept{
			for(auto s : servers)
				if( serv == s )
					return true;
			return false;
		}
		bool isXMPP(std::string & servaddr) noexcept;

	public:
		Commands(void) = default; //: reDomain(regex_dom_i2p) {}
		void InitList(void);
		void Help(gloox::Client & client, const gloox::Message & stanza) noexcept;
		void Get_list(gloox::Client & client, const gloox::Message & stanza);
		void Add_list(gloox::Client & client, const gloox::Message & stanza);
};

