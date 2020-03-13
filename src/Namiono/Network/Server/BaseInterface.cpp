#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		BaseInterface::BaseInterface()
		{
		}

		BaseInterface::~BaseInterface()
		{
		}

		bool BaseInterface::Init()
		{
			return false;
		}

		bool BaseInterface::Start()
		{
			return false;
		}

		void BaseInterface::Listen()
		{
		}

		bool BaseInterface::Stop()
		{
			return false;
		}

		bool BaseInterface::Close()
		{
			return false;
		}

		bool BaseInterface::Send()
		{
			return false;
		}
		
		void BaseInterface::Set_Gateway(const _IPADDR& gateway)
		{
			this->gateway = gateway;
		}

		void BaseInterface::Set_Address(const _IPADDR& address)
		{
			this->address = address;
		}

		_IPADDR BaseInterface::Get_Address() const
		{
			return this->address;
		}

		void BaseInterface::Set_Port(const _USHORT& port)
		{
			this->port = port;
		}

		_USHORT BaseInterface::Get_Port() const
		{
			return this->port;
		}

		_IPADDR BaseInterface::Get_Gateway() const
		{
			return this->gateway;
		}
	}
}
