#include"bot.hpp"
int main(int argc, char**argum){
	if(argc < 3) 
	{
		 std::cerr << argum[0] << " bot_i2p@serv.ru/bot " << "password" << std::endl;
		 return 1;
	}
	try{
		Bot bot{ gloox::JID(argum[1]), argum[2] };
	}catch(std::runtime_error & e){
		std::cerr << e.what() << std::endl;
	}catch(...){
		std::cerr << "Exception..." << std::endl;
	}
}
