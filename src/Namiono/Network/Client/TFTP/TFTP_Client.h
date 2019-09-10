#include <Namiono/Namiono.h>
#pragma once

namespace Namiono
{
	namespace Network
	{
		class TFTP_Client
		{
		public:
			TFTP_Client();
			~TFTP_Client();
			void Set_State(const CLIENTSTATE & state);
			const CLIENTSTATE & Get_State() const;
			bool OpenFile(const std::string & path);
			void FileSeek(const long & pos);
			void FileSeek();
			void CloseFile();
			FILE * Get_FileHandle();
			void SetCurrentBlock();
			void SetCurrentBlock(const _USHORT & block);
			const _BYTE & GetWindowSize() const;
			void SetWindowSize(const _BYTE & window);
			const _USHORT & GetBlockSize() const;
			void SetBlockSize(const _USHORT & blocksize);
			const _USHORT & GetMSFTWindow() const;
			void SetMSFTWindow(const _USHORT & window);
			const _USHORT & GetCurrentBlock() const;
			const _SIZET & GetBytesToRead() const;
			void SetBytesToRead(const _SIZET & bytes);
			const _SIZET & GetBytesRead() const;
			void SetBytesRead(const _SIZET & bytes);
			const std::string & GetFilename() const;
			void SetFilename(const std::string & filename);
		private:
			_BYTE* retries = nullptr;
			_BYTE* windowsize = nullptr;
			_USHORT* msftwindow = nullptr;
			_USHORT* block = nullptr;
			_USHORT* blocksize = nullptr;
			CLIENTSTATE* tftp_state = nullptr;
			std::string* filename = nullptr;

			_SIZET* bytesread = nullptr;
			_SIZET* bytesToRead = nullptr;
			FILE* filehandle = nullptr;
		};
	}
}
