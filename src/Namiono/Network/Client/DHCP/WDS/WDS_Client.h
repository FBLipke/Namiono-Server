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
