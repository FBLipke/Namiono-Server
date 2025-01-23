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
		TFTP_Client::TFTP_Client()
		{
			this->filename = new std::string("");
			this->block = new _USHORT(0);
			this->blocksize = new _USHORT(1024);
			this->bytesread = new _SIZET(0);
			this->bytesToRead = new _SIZET(0);
			this->msftwindow = new _USHORT(27182);
			this->retries = new _BYTE(0);
			this->tftp_state = new CLIENTSTATE(CLIENTSTATE::TFTP_INIT);
			this->windowsize = new _BYTE(1);
			this->filehandle = nullptr;
		}

		TFTP_Client::~TFTP_Client()
		{
			delete this->filename;
			this->filename = nullptr;

			delete this->block;
			this->block = nullptr;

			delete this->blocksize;
			this->blocksize = nullptr;

			delete this->bytesread;
			this->bytesread = nullptr;

			delete this->bytesToRead;
			this->bytesToRead = nullptr;

			delete this->msftwindow;
			this->msftwindow = nullptr;

			delete this->retries;
			this->retries = nullptr;

			delete this->tftp_state;
			this->tftp_state = nullptr;

			delete this->windowsize;
			this->windowsize = nullptr;

			PacketBackLog.clear();

			this->CloseFile();
		}

		void TFTP_Client::Set_State(const CLIENTSTATE& state)
		{
			*this->tftp_state = state;
		}

		const CLIENTSTATE& TFTP_Client::Get_State() const
		{
			return *this->tftp_state;
		}

		bool TFTP_Client::OpenFile(const std::string& path)
		{
			_SIZET bytes = 0;

			this->filehandle = fopen(path.c_str(), "rb");

			if (fseek(this->filehandle, 0, SEEK_END) == 0)
			{
				bytes = static_cast<_SIZET>(ftell(this->filehandle));
				rewind(this->filehandle);

				this->SetBytesToRead(bytes);
			}

			return this->GetBytesToRead() != 0;
		}

		void TFTP_Client::FileSeek(const long& pos)
		{
			fseek(this->filehandle, pos, SEEK_SET);
		}

		void TFTP_Client::FileSeek()
		{
			fseek(this->filehandle, static_cast<long>(this->GetBytesRead()), SEEK_SET);
		}

		void TFTP_Client::CloseFile()
		{
			if (this->filehandle != nullptr)
			{
				fclose(this->filehandle);
				this->filehandle = nullptr;
			}
		}

		FILE* TFTP_Client::Get_FileHandle()
		{
			return this->filehandle;
		}

		void TFTP_Client::SetCurrentBlock()
		{
			*this->block = *this->block + 1;
		}

		void TFTP_Client::SetCurrentBlock(const _USHORT& block)
		{
			*this->block = block;
		}

		const _BYTE& TFTP_Client::GetWindowSize() const
		{
			return *this->windowsize;
		}

		void TFTP_Client::SetWindowSize(const _BYTE& window)
		{
			*this->windowsize = window;
		}

		const _USHORT& TFTP_Client::GetBlockSize() const
		{
			return *this->blocksize;
		}

		void TFTP_Client::SetBlockSize(const _USHORT& blocksize)
		{
			*this->blocksize = blocksize;
		}

		const _USHORT& TFTP_Client::GetMSFTWindow() const
		{
			return *this->msftwindow;
		}

		void TFTP_Client::SetMSFTWindow(const _USHORT& window)
		{
			*this->msftwindow = window;
		}

		const _USHORT& TFTP_Client::GetCurrentBlock() const
		{
			return *this->block;
		}

		const _SIZET& TFTP_Client::GetBytesToRead() const
		{
			return *this->bytesToRead;
		}

		void TFTP_Client::SetBytesToRead(const _SIZET& bytes)
		{
			*this->bytesToRead = bytes;
		}

		const _SIZET& TFTP_Client::GetBytesRead() const
		{
			return *this->bytesread;
		}

		void TFTP_Client::SetBytesRead(const _SIZET& bytes)
		{
			if (bytes == 0)
				*this->bytesread = bytes;
			else
				*this->bytesread += bytes;
		}

		const std::string& TFTP_Client::GetFilename() const
		{
			return *this->filename;
		}

		void TFTP_Client::SetFilename(const std::string& filename)
		{
			*this->filename = filename;
		}

		void TFTP_Client::ResetState(const _USHORT& block)
		{
			if (this->PacketBackLog.find(block) != this->PacketBackLog.end())
			{
				this->SetCurrentBlock(this->PacketBackLog.at(block).Block);
				this->SetBytesRead(this->PacketBackLog.at(block).BytesRead);
				this->SetBytesToRead(this->PacketBackLog.at(block).BytesToRead);
			}
		}

		void TFTP_Client::AddToBacklog(const _USHORT& block, const _SIZET& bytesRead, const _SIZET& bytesToRead)
		{
			this->PacketBackLog.emplace(block, TFTP_BackLogEntry(block, bytesRead, bytesToRead));
		}
	}
}
