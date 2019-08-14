#pragma once
#include <map>
namespace bhop_api
{
	struct user_token 
	{
		unsigned int token_length;
		char* pToken;

		unsigned int login_length;
		char* pLogin;

		unsigned int hwid_length;
		char* pHwid;
	};

	extern user_token token;
	extern bool verification_completed;
	
	extern bool hash_subscribe;
	extern int subscribe_end_date;
	extern bool skins_updated;

	extern int knife_model;
	extern int knife_skin;

	extern int glove_model;
	extern int glove_skin;

	extern std::map<int, int> skins;

	bool loader_verify();


}