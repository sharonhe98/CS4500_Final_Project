#pragma once
#include "message.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class NodeInfo : public Object {
	public:
		unsigned id;
		sockaddr_in address;
};

class NetworkIP : public NetworkIfc {
	public:
		NodeInfo* nodes_; // a collection of all the nodes in the network
		size_t num_nodes;
		size_t current_node; // current node index
		int sock_; // the socket
		sockaddr_in ip_; // the current ip address

	NetworkIP() {
		nodes_ = new NodeInfo[0];
		num_nodes = 0;
		current_node = 0;
	}

	~NetworkIP() { close(sock_); }

	size_t index() override {return current_node; }

	void init_sock_(size_t port) {
		assert((sock_ = sock(AF_INET, SOCK_STREAM, 0)) >= 0);
		int opt = 1;
		assert(setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == 0);
		ip_.sin_family = AF_INET;
		ip_.sin_addr.s_addr = INADDR_ANY;
		ip_.sin_port = htons(port);
		assert(bind(sock_, (sockaddr*)&ip_, sizeof(ip_)) >= 0);
		assert(listen(sock_, 100) >= 0);	
	}

	// initializes the server/registration node to node 0
	void server_init(unsigned idx, unsigned port) {
		current_node = idx;
		init_sock_(port);
		if (num_nodes > 0) { // assume we only add client nodes to the directory
			Directory ipd(ports, addresses);
			for (size_t i = 1; i < num_nodes; i++) {
				ipd.target_ = i;
				send_m(&ipd);
			}
		}
	}

	void client_init(unsigned idx, unsigned port, char* server_addr, unsigned server_port) {
		current_node = idx;
		init_sock(port);
		nodes_ = new NodeInfo[num_nodes + 1];
		nodes_[num_nodes].id = 0;
		nodes_[num_nodes].address.sin_family = AF_INET;
		nodes_[num_nodes].address.sin_port = htons(server_port);
		if (inet_pton(AF_INET, server_adr, &nodes_[num_nodes].address.sin_addr) <= 0) {
			assert(false && "Invalid server IP address format");
		}
		Register msg(idx, port);
		send_m(&msg);
		Directory* ipd = dynamic_cast<Directory*>(recv_m());
		//ipd->log();
		for (size_t i = 0; i < ipd->clients; i++) {
			nodes_[i].id = i + 1;
			nodes_[i].address.sin_family = AF_INET;
			nodes_[i].address.sin_port = htons(ipd->ports[i]);
			if (inet_pton(AF_INET, ipd->addresses[i]->c_str(), &nodes_[i].address.sin_addr) <= 0) {
				printf("Invalid IP directory-addr for node %zu", (i + 1));
				exit(1);
			}
		}
	}

	void send_m(Message* msg) override {
		NodeInfo &target = nodes_[msg->getTarget()];
		int connected = socket(AF_INET, SOCK_STREAM, 0);
		assert(connected >= 0 && "Unable to create client socket");
		if (connect(connected, (sockaddr*)&target.address, sizeof(target.address)) < 0) {
			perror("Unable to connect to remote node :(");
		}
		Serializer ser;
		char* buffer = msg->serialize(ser);
		size_t size = ser.size();
		send(connected, &size, sizeof(size_t), 0);
		send(connected, buffer, size, 0);
	}

	Message* recv_m() override {
		sockaddr_in sender;
		socklen_t addrlen = sizeof(sender);
		int req = accept(sock_, (sockaddr*)&sender, &addrlen);
		size_t size = 0;
		if (read(req, &size, sizeof(size_t)) == 0) {
			perror("failed to read");
			exit(1);
		}
		char* buffer = new char[size];
		int rd = 0;
		while (rd != size) {
			rd += read(req, buffer + rd, size - rd);
		}
		Deserializer des(buffer, size);
		Message* msg = Message::deserialize(s, sender);
		return msg;
	}	
}
