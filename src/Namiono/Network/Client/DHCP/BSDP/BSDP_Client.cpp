#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		BSDP_Client::BSDP_Client()
		{
			this->MSGTYPE = new BSDP_MSGTYPE(LIST);
			this->Priority = new _USHORT(0x0a);
			this->BootImage = new _UINT(1);
			this->SelectedImage = new _UINT(1);
			this->attributes = new BSDP_ATTIBUTE(Install);
			this->ServerIdent = new _IPADDR(0);
			this->ReplyPort = new _USHORT(0);
		}

		BSDP_Client::~BSDP_Client()
		{
			delete this->attributes;
			this->attributes = nullptr;

			delete this->BootImage;
			this->BootImage = nullptr;

			delete this->Priority;
			this->Priority = nullptr;

			delete this->MSGTYPE;
			this->MSGTYPE = nullptr;

			delete this->ServerIdent;
			this->ServerIdent = nullptr;

			delete this->ReplyPort;
			this->ReplyPort = nullptr;
		}

		void BSDP_Client::Set_ServerIdent(const _IPADDR& ident)
		{
			*this->ServerIdent = ident;
		}

		void BSDP_Client::Set_BootImage(const _UINT& id)
		{
			*this->BootImage = id;
		}

		_UINT BSDP_Client::Get_BootImage() const
		{
			return *this->BootImage;
		}

		void BSDP_Client::Set_SelectedImage(const _UINT& id)
		{
			*this->SelectedImage = id;
		}

		_UINT BSDP_Client::Get_SelectedImage() const
		{
			return *this->SelectedImage;
		}

		bool BSDP_Client::Get_ServerIdent() const
		{
			return *this->ServerIdent;
		}

		void BSDP_Client::Set_ReplyPort(const _USHORT& port)
		{
			*this->ReplyPort = port;
		}

		_USHORT BSDP_Client::Get_ReplyPort() const
		{
			return *this->ReplyPort;
		}

		void BSDP_Client::Set_Attributes(const BSDP_ATTIBUTE& attribs)
		{
			*this->attributes = attribs;
		}

		BSDP_ATTIBUTE BSDP_Client::Get_Attributes() const
		{
			return *this->attributes;
		}
	}
}
