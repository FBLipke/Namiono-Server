/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		WDS_Client::WDS_Client()
		{
			this->NextAction = new _BYTE(WDSNBP_OPTION_NEXTACTION::APPROVAL);
			this->ActionDone = new WDSNBP_ActionDone_Values(True);
			this->PollIntervall = new _USHORT(10);
			this->RetryCount = new _USHORT(65535);
			this->requestid = new _ULONG(1);
			this->bcdfile = new std::string("");
			this->referalIP = new _IPADDR(0);
			this->AdminMessage = new std::string("");
			this->ServerSelection = new bool(0);
		}

		std::string& WDS_Client::GetBCDfile() const
		{
			return *this->bcdfile;
		}

		void WDS_Client::SetReferralServer(const _IPADDR& addr)
		{
			*this->referalIP = addr;
		}

		const _IPADDR& WDS_Client::GetReferalServer() const
		{
			return *this->referalIP;
		}

		void WDS_Client::SetRequestID(const _ULONG& id)
		{
			*this->requestid = id;
		}

		const _ULONG& WDS_Client::GetRequestID() const
		{
			return *this->requestid;
		}

		void WDS_Client::SetActionDone(const WDSNBP_ActionDone_Values& done)
		{
			*this->ActionDone = done;
		}

		const WDSNBP_ActionDone_Values& WDS_Client::GetActionDone() const
		{
			return *this->ActionDone;
		}

		void WDS_Client::SetWDSMessage(const std::string& message)
		{
			*this->AdminMessage = message;
		}

		const std::string& WDS_Client::GetWDSMessage() const
		{
			return *this->AdminMessage;
		}

		void WDS_Client::SetNextAction(const WDSNBP_OPTION_NEXTACTION& action)
		{
			*this->NextAction = action;
		}

		const _BYTE& WDS_Client::GetNextAction() const
		{
			return *this->NextAction;
		}

		void WDS_Client::SetRetryCount(const _USHORT& action)
		{
			*this->RetryCount = action;
		}

		const _USHORT& WDS_Client::GetRetryCount() const
		{
			return *this->RetryCount;
		}

		void WDS_Client::SetPollInterval(const _USHORT& interval)
		{
			*this->PollIntervall = interval;
		}

		const _USHORT& WDS_Client::GetPollInterval() const
		{
			return *this->PollIntervall;
		}

		void WDS_Client::SetBCDfile(const std::string& file)
		{
			*this->bcdfile = file;
		}

		WDS_Client::~WDS_Client()
		{
			delete this->NextAction;
			this->NextAction = nullptr;

			delete this->ActionDone;
			this->ActionDone = nullptr;

			delete this->PollIntervall;
			this->PollIntervall = nullptr;

			delete this->RetryCount;
			this->RetryCount = nullptr;

			delete this->requestid;
			this->requestid = nullptr;

			delete this->bcdfile;
			this->bcdfile = nullptr;

			delete this->referalIP;
			this->referalIP = nullptr;

			delete this->AdminMessage;
			this->AdminMessage = nullptr;

			delete this->ServerSelection;
			this->ServerSelection = nullptr;
		}
	}
}
