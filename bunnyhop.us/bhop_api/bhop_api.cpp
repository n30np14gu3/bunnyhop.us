#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "bhop_api.h"

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>

#include <Windows.h>
#include <wincrypt.h>
#include "../SDK/crypto/hash/sha256.hpp"
#include <string>

#include "../SDK/crypto/XorStr.h"

class local_client
{
public:
	int NoError = 0;

	local_client(const char* ip, u_short port);
	~local_client();

	byte* recivepacket(DWORD len, DWORD* lpRecived);
	bool sendpacket(byte* data, DWORD data_size);
private:

	struct SERVER_RESPONSE
	{
		DWORD user_id;
		byte hwid[65];
	};


	WSADATA m_wsaData{};
	SOCKET m_sClient;
	sockaddr_in m_sockAddr{};
	int m_iErrorCode;

};


local_client::local_client(const char* ip, u_short port)
{
	m_sClient = 0;
	m_iErrorCode = 0;

	NoError = !WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (!NoError)
		return;

	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ZeroMemory(&m_sockAddr, sizeof(sockaddr_in));
	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr.s_addr = inet_addr(ip);
	m_sockAddr.sin_port = htons(port);

	m_iErrorCode = connect(m_sClient, reinterpret_cast<sockaddr*>(&m_sockAddr), sizeof(m_sockAddr));

	NoError = m_iErrorCode != SOCKET_ERROR;
	if (!NoError)
	{
		closesocket(m_sClient);
		WSACleanup();
	}
}

byte* local_client::recivepacket(DWORD len, DWORD* lpRecived)
{
	byte* buff = new byte[len];
	memset(buff, 0, len);
	*lpRecived = recv(m_sClient, reinterpret_cast<char*>(buff), len, 0);
	return buff;
}

bool local_client::sendpacket(byte* data, DWORD data_size)
{
	return  static_cast<DWORD>(send(m_sClient, reinterpret_cast<const char*>(data), data_size, 0)) == data_size;
}

local_client::~local_client()
{
	NoError = 0;
	m_sClient = 0;
	m_iErrorCode = 0;
	ZeroMemory(&m_sockAddr, sizeof(sockaddr_in));
	closesocket(m_sClient);
	WSACleanup();

}


namespace bhop_api
{
	user_token token{};
	bool verification_completed{ false };
	bool hash_subscribe{ false };

	int awp_skin{ 0 };

	int glove_skin{ 0 };
	int glove_model{ 0 };

	int knife_skin{ 0 };
	int knife_model{ 0 };

	bool skins_updated{ false };

}

bool bhop_api::loader_verify()
{
	local_client client(XorStr("127.0.0.1"), 1548);
	if (!client.NoError)
		return false;

	byte key[8] = { 0 };
	HCRYPTPROV hCryptCtx = NULL;
	CryptAcquireContext(&hCryptCtx, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptGenRandom(hCryptCtx, 8, key);
	CryptReleaseContext(hCryptCtx, 0);
	client.sendpacket(key, 8);


	DWORD lpRecived = 0;
	DWORD* size = (DWORD*)client.recivepacket(4, &lpRecived);
	byte* token = client.recivepacket(*size, &lpRecived);
	byte* token_hashed = new byte[*size];
	DWORD tokenSize = *size;
	memcpy(token_hashed, token, *size);
	if (token == nullptr)
		return false;

	for (DWORD i = 0; i < *size; i++)
	{
		token[i] ^= key[i % 4];
		token_hashed[i] ^= key[i % 4 + 4];
	}

	byte* hash = client.recivepacket(65, &lpRecived);
	std::string h1 = std::string((const char*)hash);
	std::string h2 = sha256(token_hashed, tokenSize);

	if (h1 != h2)
		return false;

	bhop_api::token.token_length = tokenSize;
	bhop_api::token.pToken = new char[tokenSize];
	memcpy(bhop_api::token.pToken, token, tokenSize);
	bhop_api::verification_completed = true;
	return true;
}

