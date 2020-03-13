#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		BaseServer::BaseServer()
		{
		}

		BaseServer::~BaseServer()
		{
		}

		bool BaseServer::Init()
		{
			return false;
		}
		
		bool BaseServer::Start()
		{
			return false;
		}
		
		void BaseServer::Listen()
		{
		}
		
		bool BaseServer::Stop()
		{
			return false;
		}
		
		bool BaseServer::Close()
		{
			return false;
		}

		bool BaseServer::Send()
		{
			return false;
		}
	}
}
