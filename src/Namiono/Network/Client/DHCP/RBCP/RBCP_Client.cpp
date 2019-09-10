#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		RBCP_Client::RBCP_Client()
		{
			this->item = new _USHORT(0);
			this->layer = new RBCP_LAYER(RBCP_LAYER::Bootfile);

			this->mcastip = new _IPADDR(0);
			this->control = new RBCP_DISCOVERYCONTROL(DISABLE_MCAST);
		}

		void RBCP_Client::Set_Item(const _USHORT& item)
		{
			*this->item = item;
		}

		const _USHORT& RBCP_Client::Get_Item() const
		{
			return *this->item;
		}

		void RBCP_Client::Set_Layer(const RBCP_LAYER& layer)
		{
			*this->layer = layer;
		}

		const RBCP_LAYER& RBCP_Client::Get_Layer() const
		{
			return *this->layer;
		}

		void RBCP_Client::Set_MulticastIP(const _IPADDR& ip)
		{
			*this->mcastip = ip;
		}

		const _IPADDR& RBCP_Client::Get_MulticastIP() const
		{
			return *this->mcastip;
		}

		RBCP_Client::~RBCP_Client()
		{
			delete this->item;
			this->item = nullptr;

			delete this->layer;
			this->layer = nullptr;

			delete this->control;
			this->control = nullptr;

			delete this->mcastip;
			this->mcastip = nullptr;
		}
	}
}
