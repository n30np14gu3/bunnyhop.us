#pragma once
namespace bhop_api
{
	struct user_token 
	{
		unsigned int token_length;
		char* pToken;
	};

	extern user_token token;
	extern bool verification_completed;
	extern bool hash_subscribe;

	extern int awp_skin;

	extern int glove_skin;
	extern int glove_model;

	extern int knife_skin;
	extern int knife_model;

	extern bool skins_updated;

	bool loader_verify();


}