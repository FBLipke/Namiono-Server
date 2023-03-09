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

#include <Environment/environment.h>
#pragma once

namespace Namiono
{
	namespace Network
	{
		typedef enum WDSNBP_Options
		{
			WDSBP_OPT_ARCHITECTURE = 1,
			WDSBP_OPT_NEXT_ACTION = 2,
			WDSBP_OPT_POLL_INTERVAL = 3,
			WDSBP_OPT_POLL_RETRY_COUNT = 4,
			WDSBP_OPT_REQUEST_ID = 5,
			WDSBP_OPT_MESSAGE = 6,
			WDSBP_OPT_VERSION_QUERY = 7,
			WDSBP_OPT_SERVER_VERSION = 8,
			WDSBP_OPT_REFERRAL_SERVER = 9,
			WDSBP_OPT_PXE_CLIENT_PROMPT = 11,
			WDSBP_OPT_PXE_PROMPT_DONE = 12,
			WDSBP_OPT_NBP_VER = 13,
			WDSBP_OPT_ACTION_DONE = 14,
			WDSBP_OPT_ALLOW_SERVER_SELECTION = 15,
			WDSBP_OPT_SERVER_FEATURES = 16,
			WDSBP_OPT_END = 0xff
		} WDSNBP_Options;

		typedef enum WDSNBP_OPTION_NEXTACTION
		{
			APPROVAL = 0x01,
			REFERRAL = 0x03,
			ABORT = 0x05
		} WDSNBP_OPTION_NEXTACTION;

		typedef enum WDSNBP_ActionDone_Values
		{
			False = 0x00,
			True = 0x01
		} WDSNBP_NEXTACTION_Values;
	}
}
