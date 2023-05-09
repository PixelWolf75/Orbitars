#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
//listen for packets
//get them
//send them for processing

typedef std::function<void(void* pData, size_t len, const void* pWinsockAddr, int WTF, void* pContext)> receive_event;

namespace API
{
	namespace Objects
	{
		class UDPState {
		public:
			std::mutex m;
			std::condition_variable cv;
			std::unique_lock<std::mutex> lk;
			bool is_quiting;
			std::shared_ptr<void> pSharedState;
			void* pState;
			UDPState()
				: lk(m)
				, is_quiting(false)
			{

			}
			~UDPState() {
				delete pState; // TODO:  unique pointer?
			}


			void wait() {

				// Wait for termination
				cv.wait(lk, [&]() -> bool { return is_quiting; });
			}
		};
		class UDPInterface
		{
		public:
			virtual ~UDPInterface() = default;
			virtual void Send(const void* pData, size_t len, const void* pDest) = 0;
			// Sends packet across
			// params:
			// pData: raw data to be sent as a string of byte has no ownership
			// len: size/amount of data sent
			// pDest: Socket address of the socet we are sending towars
			virtual void set_callback(receive_event r, void* pNewContext) = 0;
			// Sets new call back for the server to connect to
			// r: new callback of the client to connect to
		};

	}

	namespace Factories
	{
		class UDPFactory
		{
		public:
			static std::unique_ptr<Objects::UDPInterface> create_server(int iPort, receive_event r, void* Context);
			static std::unique_ptr<Objects::UDPInterface> create_client(std::string host, int iPort, receive_event r, void* Context, std::string ID);
		};
	}

}