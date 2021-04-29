
/* $Id$
 * EOSERV is released under the zlib license.
 * See LICENSE.txt for more info.
 */

#ifndef EOSERVER_HPP_INCLUDED
#define EOSERVER_HPP_INCLUDED

#include "fwd/eoserver.hpp"

#include "fwd/config.hpp"
#include "fwd/database.hpp"
#include "fwd/eoclient.hpp"
#include "fwd/sln.hpp"
#include "fwd/world.hpp"

#include "socket.hpp"

#include <array>
#include <string>
#include <unordered_map>

void server_ping_all(void *server_void);
void server_pump_queue(void *server_void);

/**
 * A server which accepts connections and creates EOClient instances from them
 */
class EOServer : public Server
{
	private:
		std::unordered_map<IPAddress, double, std::hash<IPAddress>> connection_log;
		void Initialize(const Config &eoserv_config, const Config &admin_config);
		void Initialize(std::unique_ptr<Database>&& database, const Config &eoserv_config, const Config &admin_config);
		void InitializeShared();

	protected:
		virtual Client *ClientFactory(const Socket &);

	public:
		World *world;
		double start;
		SLN *sln;

		EOServer(IPAddress addr, unsigned short port, const Config &eoserv_config, const Config &admin_config) : Server(addr, port)
		{
			this->Initialize(eoserv_config, admin_config);
		}

		EOServer(IPAddress addr, unsigned short port, std::unique_ptr<Database>&& database, const Config &eoserv_config, const Config &admin_config) : Server(addr, port)
		{
			this->Initialize(std::move(database), eoserv_config, admin_config);
		}

		void Tick();

		~EOServer();
};

#endif // EOSERVER_HPP_INCLUDED
