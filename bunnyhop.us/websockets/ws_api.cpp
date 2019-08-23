#include "ws_api.h"
#include "../bhop_api/bhop_api.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>


#if !NDEBUG
#include <Windows.h>
#endif

bhop_api::user_token* USER_DATA = nullptr;

void apply_skins(const rapidjson::Value& skins)
{
		
	bhop_api::knife_skin = 0;
	bhop_api::knife_model = 0;

	bhop_api::glove_skin = 0;
	bhop_api::glove_model = 0;

	bhop_api::skins.clear();

	bhop_api::knife_skin = skins["knife"]["skin"].GetInt();
	bhop_api::knife_model = skins["knife"]["model"].GetInt();

	bhop_api::glove_skin = skins["glove"]["skin"].GetInt();
	bhop_api::glove_model = skins["glove"]["model"].GetInt();


	int weapon_count = skins["weapon"]["count"].GetInt();
	if (weapon_count != 0)
	{
		rapidjson::GenericArray weapon_skins = skins["weapon"]["skins"].GetArray();
		for (int i = 0; i < weapon_count; i++)
		{
			bhop_api::skins[weapon_skins[i]["weapon_id"].GetInt()] = weapon_skins[i]["skin_id"].GetInt();
		}
	}
	bhop_api::skins_updated = true;
}

namespace ws_api
{
	namespace beast = boost::beast;         // from <boost/beast.hpp>
	namespace http = beast::http;           // from <boost/beast/http.hpp>
	namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
	namespace net = boost::asio;            // from <boost/asio.hpp>
	using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


	// Sends a WebSocket message and prints the response
	class session : public std::enable_shared_from_this<session>
	{
		tcp::resolver resolver_;
		websocket::stream<beast::tcp_stream> ws_;
		beast::flat_buffer buffer_;
		std::string host_;
		std::string api_;

	public:
		// Resolver and socket require an io_context
		explicit
			session(net::io_context& ioc)
			: resolver_(net::make_strand(ioc))
			, ws_(net::make_strand(ioc))
		{
		}

		// Start the asynchronous operation
		void
			run(
				char const* host,
				char const* port,
				char const* api)
		{
			// Save these for later
			host_ = host;
			api_ = api;

#if !NDEBUG
			MessageBox(nullptr, "Running", "", MB_OK);
#endif

			// Look up the domain name
			resolver_.async_resolve(
				host,
				port,
				beast::bind_front_handler(
					&session::on_resolve,
					shared_from_this()));
		}

		void
			on_resolve(
				beast::error_code ec,
				tcp::resolver::results_type results)
		{
			if (ec)
				exit(0);

			// Set the timeout for the operation
			beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

			// Make the connection on the IP address we get from a lookup
			beast::get_lowest_layer(ws_).async_connect(
				results,
				beast::bind_front_handler(
					&session::on_connect,
					shared_from_this()));
		}

		void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
		{
#if !NDEBUG
			MessageBox(nullptr, "OnConnect Event", "", MB_OK);
#endif
			if (ec)
				exit(0);


			beast::get_lowest_layer(ws_).expires_never();

			ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

			ws_.set_option(websocket::stream_base::decorator(
				[](websocket::request_type& req)
				{
					req.set(http::field::sec_websocket_protocol, "bunnyhop");
				}));

			ws_.async_handshake(host_, api_.c_str(), beast::bind_front_handler(&session::on_handshake, shared_from_this()));
		}

		void on_handshake(beast::error_code ec)
		{
#if !NDEBUG
			MessageBox(nullptr, "OnHandshake Event", "", MB_OK);
#endif
			if (ec)
				exit(0);

			rapidjson::Document request;
			request.SetObject();
			rapidjson::Document::AllocatorType& allocator = request.GetAllocator();

			request.AddMember("type", "verifytoken", allocator);

			rapidjson::Value requestData;
			requestData.SetObject();
			requestData.AddMember("token", rapidjson::StringRef(USER_DATA->pToken, bhop_api::token.token_length - 1), allocator);
			requestData.AddMember("login", rapidjson::StringRef(USER_DATA->pLogin, bhop_api::token.login_length - 1), allocator);
			requestData.AddMember("hwid", rapidjson::StringRef(USER_DATA->pHwid, bhop_api::token.hwid_length - 1), allocator);

			request.AddMember("data", requestData, allocator);

			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			request.Accept(writer);

			std::string json = std::string(buffer.GetString());
			ws_.async_write(
				net::buffer(json),
				beast::bind_front_handler(
					&session::on_write,
					shared_from_this()));
		}

		void on_write(beast::error_code ec, std::size_t bytes_transferred)
		{
			boost::ignore_unused(bytes_transferred);

			if (ec)
				exit(0);

			ws_.async_read(
				buffer_,
				beast::bind_front_handler(
					&session::on_read,
					shared_from_this()));
		}

		void on_read(beast::error_code ec, std::size_t bytes_transferred)
		{
#if !NDEBUG
			MessageBox(nullptr, "OnRead Event", "", MB_OK);
#endif

			boost::ignore_unused(bytes_transferred);

			if (ec)
				exit(0);

			std::string s = beast::buffers_to_string(buffer_.data());

#if !NDEBUG
			MessageBox(nullptr, s.c_str(), "", MB_OK);
#endif

			buffer_.clear();
			rapidjson::Document d;
			d.Parse(s.c_str());
			std::string type = std::string(d["type"].GetString());
			if (type == "online")
			{
				ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
				return;
			}

			if (type == "verifytoken")
			{
				int status = d["data"]["status"].GetInt();
				if (status != 1)
					exit(0);

				bhop_api::hash_subscribe = d["data"]["subscribe"].GetBool();
				bhop_api::subscribe_end_date = d["data"]["dateEnd"].GetInt();
				
				if (bhop_api::hash_subscribe)
					apply_skins(d["data"]["skins"]);
				ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
				return;
			}

			if (type == "updateSkins")
			{
				if (bhop_api::hash_subscribe)
					apply_skins(d["data"]["skins"]);

				ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
				return;

			}

			ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
			//ws_.async_close(websocket::close_code::normal,
			//	beast::bind_front_handler(
			//		&session::on_close,
			//		shared_from_this()));
		}

		void on_close(beast::error_code ec)
		{
			if (ec)
				exit(0);

			std::cout << beast::make_printable(buffer_.data()) << std::endl;
		}
	};
}


void ws_api::init_client(const char* host, const char* port, const char* api, const void* pUserData)
{
	net::io_context ioc;
	USER_DATA = (bhop_api::user_token*)pUserData;
	std::make_shared<session>(ioc)->run(host, port, api);
	ioc.run();
}