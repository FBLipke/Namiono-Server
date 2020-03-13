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
#pragma once

namespace Namiono
{
	namespace Network
	{
		class WDS_Client
		{
		public:
			WDS_Client();
			~WDS_Client();

			std::string& GetBCDfile() const;
			void SetReferralServer(const _IPADDR& addr);
			const _IPADDR& GetReferalServer() const;
			void SetRequestID(const _ULONG& id);
			const _ULONG& GetRequestID() const;
			void SetActionDone(const WDSNBP_ActionDone_Values& done);
			const WDSNBP_ActionDone_Values& GetActionDone() const;
			void SetWDSMessage(const std::string& message);
			const std::string& GetWDSMessage() const;
			void SetNextAction(const WDSNBP_OPTION_NEXTACTION& action);
			const _BYTE& GetNextAction() const;
			void SetRetryCount(const _USHORT& action);
			const _USHORT& GetRetryCount() const;
			void SetPollInterval(const _USHORT& interval);
			const _USHORT& GetPollInterval() const;
			void SetBCDfile(const std::string& file);
		private:
			_BYTE* NextAction = nullptr;
			WDSNBP_ActionDone_Values* ActionDone = nullptr;
			_USHORT* PollIntervall = nullptr;
			_USHORT* RetryCount = nullptr;
			_ULONG* requestid = nullptr;
			std::string* bcdfile = nullptr;
			_IPADDR* referalIP = nullptr;
			std::string* AdminMessage = nullptr;
			bool* ServerSelection = nullptr;
		};
	}
}
