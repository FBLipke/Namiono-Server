#include <Namiono/Namiono.h>
#pragma once

namespace Namiono
{
	namespace Network
	{
		class RBCP_Client
		{
		public:
			RBCP_Client();
			void Set_Item(const _USHORT & item);
			const _USHORT & Get_Item() const;
			void Set_Layer(const RBCP_LAYER & layer);
			const RBCP_LAYER & Get_Layer() const;
			void Set_MulticastIP(const _IPADDR & ip);
			const _IPADDR & Get_MulticastIP() const;
			~RBCP_Client();
		private:
			_USHORT* item = nullptr;
			RBCP_LAYER* layer = nullptr;

			_IPADDR* mcastip = nullptr;
			RBCP_DISCOVERYCONTROL* control = nullptr;
		};
	}
}
