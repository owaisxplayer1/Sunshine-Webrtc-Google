#pragma once

namespace signalling {
	using DelegateOnWSMessage =
		void (*)(std::string&);

	extern DelegateOnWSMessage ws_onMessageCB;

	void StartWebsocket();
	void RegisterOnMessage(DelegateOnWSMessage cb);

	void SendMessage(char* msg, size_t size);
	void SendMessage(std::string msg);

	std::string ReadMessage();
}