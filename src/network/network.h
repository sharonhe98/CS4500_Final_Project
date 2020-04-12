#pragma once
#include "network_ifc.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

class NodeInfo : public Object
{
public:
	size_t id;
	sockaddr_in address;
};

class NetworkIP : public NetworkIfc
{
public:
	NodeInfo *nodes_; // a collection of all the nodes in the network
	size_t num_nodes;
	size_t current_node; // current node index
	int sock_;			 // the socket
	char* ipstring;		// the current ip address string
	struct sockaddr_in ip_;	 // the current ip address
	IntArray* ports;
	StringArray* addresses;

	NetworkIP(char* ip, size_t total_nodes, size_t idx)
	{
		ipstring = ip;
		ports = new IntArray();
		num_nodes = total_nodes;
		nodes_ = new NodeInfo[num_nodes];
		current_node = idx;
		addresses = new StringArray();
	}

	~NetworkIP() {
		delete [] ports;
		delete [] addresses;
		close(sock_); 
	}

	size_t index() override { return current_node; }

	void init_sock_(size_t port)
	{
		assert((sock_ = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
		int opt = 1;
		assert(setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == 0);
		ip_.sin_family = AF_INET;
		ip_.sin_addr.s_addr = INADDR_ANY;
		ip_.sin_port = htons(port);
		assert(bind(sock_, (struct sockaddr *)&ip_, sizeof(ip_)) >= 0);
		//if (bind(sock_, (struct sockaddr *)&ip_, sizeof(ip_)) < 0); perror("bind failed!\n");
		assert(listen(sock_, num_nodes) >= 0);
	}

	// initializes the server/registration node to node 0
	void server_init(unsigned idx, unsigned port)
	{
		current_node = idx;
		init_sock_(port);
		// server knows how many nodes
		for (size_t i = 0; i < num_nodes; i++) {
			nodes_[0].address = ip_;
			nodes_[0].id = 0;
		}	
		for (size_t i = 1; i < num_nodes; i++)
		{
			Register *msg = dynamic_cast<Register*>(recv_m());
			size_t port = msg->port_;
			size_t sender = msg->getSender();
			nodes_[sender].id = sender;
			nodes_[sender].address.sin_family = AF_INET;
			nodes_[sender].address.sin_port = htons(port);
			printf("node %zu is registering\n", i);
		}
		for (size_t i = 0; i < num_nodes - 1; ++i) {
			ports->set(i, ntohs(nodes_[i+1].address.sin_port));
			String* address = new String(inet_ntoa(nodes_[i+1].address.sin_addr));
			addresses->set(i, address);
		}
		for (size_t i = 1; i < num_nodes; i++) {
			Directory ipd(MsgKind::Directory, current_node, i, nodes_[i].id, i, ports, addresses);
			ipd.target_ = i;
			send_m(&ipd);
		
		}
		printf("Server has been initialized!\n");


	}

	void client_init(unsigned idx, unsigned port, char *server_addr, unsigned server_port)
	{
		current_node = idx;
		init_sock_(port);

		nodes_[0].id = 0;
		nodes_[0].address.sin_family = AF_INET;
		nodes_[0].address.sin_port = htons(server_port);

		if (inet_pton(AF_INET, server_addr, &nodes_[0].address.sin_addr) <= 0)
		{
			assert(false && "Invalid server IP address format");
		}
		Register* msg = new Register(MsgKind::Register, idx, 0, 42, ip_, port);
		send_m(msg);


		Directory *ipd = dynamic_cast<Directory *>(recv_m());
		NodeInfo *nodes = new NodeInfo[num_nodes];
		nodes[0] = nodes_[0];

		for (size_t i = 0; i < ipd->addresses_->length(); ++i)
		{
			nodes_[i+1].id = i + 1;
			nodes_[i+1].address.sin_family = AF_INET;
			nodes_[i+1].address.sin_port = htons(ipd->ports_->get(i));
			if (inet_pton(AF_INET, ipd->addresses_->get(i)->c_str(), &nodes_[i+1].address.sin_addr) <= 0)
			{
				perror("Invalid IP directory-addr for node");
				exit(1);
			}
		}
		delete[] nodes_;
		nodes_ = nodes;
		printf("Client has registered!\n");

	}

	void send_m(Message *msg) override
	{
		NodeInfo &target = nodes_[msg->getTarget()];

		int connected = socket(AF_INET, SOCK_STREAM, 0);
		assert(connected >= 0 && "Unable to create client socket");
		if (connect(connected, (sockaddr *)&target.address, sizeof(target.address)) < 0)
		{
			perror("Unable to connect to remote node :(");
		}
		Serializer* ser = new Serializer();
		msg->serialize(ser);
		char *buffer = ser->getSerChar();
		size_t size = ser->getPos();
		int status = send(connected, buffer, size, 0);
		assert(status >= 0);
	}

	Message *recv_m() override
	{
		sockaddr_in sender;
		socklen_t addrlen = sizeof(sender);
		int req = accept(sock_, (sockaddr *)&sender, &addrlen);
		size_t size = 0;
		if (read(req, &size, sizeof(size_t)) == 0)
		{
			perror("failed to read");
			exit(1);
		}
		char *buffer = new char[size];
		int rd = 0;
		while (rd != size)
		{
			rd += read(req, buffer + rd, size - rd);
		}

		Deserializer* des = new Deserializer(buffer);
		Message *msg = Message::deserializeMsg(des);
		return msg;
	}
};
