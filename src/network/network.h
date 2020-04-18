#pragma once
#include "network_ifc.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

/** Class represents a Node*/
class NodeInfo : public Object
{
public:
	size_t id; // node id
	sockaddr_in address; // node sock address
};

/** Class that represents the implementation of the Network Interface*/
class NetworkIP : public NetworkIfc
{
public:
	NodeInfo *nodes_; // a collection/directory of all the nodes in the network system
	size_t num_nodes; // number of nodes in network system
	size_t current_node; // this node index
	int sock_;			 // the socket
	char* ipstring;		// the current ip address string
	struct sockaddr_in ip_;	 // this node ip address
	IntArray* ports;	// list of ports in the network system
	StringArray* addresses;	// list of addresses in the network system

	// network constructor
	NetworkIP(char* ip, size_t total_nodes, size_t idx)
	{
		ipstring = ip;
		ports = new IntArray();
		num_nodes = total_nodes;
		// nodes_ = new NodeInfo[num_nodes];
		current_node = idx;
		addresses = new StringArray();
	}

	// network destructor
	~NetworkIP() {
		delete [] ports;
		delete [] addresses;
		close(sock_);	// close socket
	}

	// Returns this node's index
	size_t index() override { return current_node; }

	// Initialize the sock
	void init_sock_(size_t port)
	{
		assert((sock_ = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
		int opt = 1;
		assert(setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == 0);
		ip_.sin_family = AF_INET;
		ip_.sin_addr.s_addr = INADDR_ANY;
		ip_.sin_port = htons(port);
		assert(bind(sock_, (struct sockaddr *)&ip_, sizeof(ip_)) >= 0);
		// if (bind(sock_, (struct sockaddr *)&ip_, sizeof(ip_)) < 0); perror("bind failed!\n");
		assert(listen(sock_, 100) >= 0);
	}

	// Initializes the server/registration node to node 0.
	// Initializes state and wait for connection Register messages from clients
	// Once server receives all of clients' Register messages, we have a Directory message we can then send to the clients
	void server_init(unsigned idx, unsigned port)
	{
		current_node = idx;	// set this node as server idx
		init_sock_(port);	// initialize sock address info
		nodes_ = new NodeInfo[num_nodes];	// initialize directory of nodes

		// initialize each Node ids
		for (size_t i = 0; i < num_nodes; i++)
		{
			nodes_[i].id = 0;
		}
		// set the server node address
		nodes_[0].address = ip_;

		// loop over number of client nodes
		// receives message and casts to Register message
		// grabs the data from Register and set the client Node Info
		for (size_t i = 1; i < num_nodes; ++i) {
			Register* regMsg = dynamic_cast<Register*>(recv_m());
			nodes_[regMsg->getSender()].id = regMsg->getSender();
			nodes_[regMsg->getSender()].address.sin_family = AF_INET;
			nodes_[regMsg->getSender()].address.sin_addr = regMsg->client_.sin_addr;
			nodes_[regMsg->getSender()].address.sin_port = htons(regMsg->port_);
		}
		// after client nodes are registered
		// add each client port and address to list of ports and addresses 
		for (size_t i = 0; i < num_nodes; i++) {
			ports->append(ntohs(nodes_[i+1].address.sin_port));
			String * addr = new String(inet_ntoa(nodes_[i+1].address.sin_addr));
			addresses->append(addr);
		}
		// at the end of server init, we create a Directory that contains ports and addresses of all the nodes in the system
		// send it to each target / client node
		for (size_t i = 1; i < num_nodes; i++) {
			Directory ipd(MsgKind::Directory, current_node, i, 24, num_nodes-1, ports, addresses);
			send_m(&ipd);
		}
		printf("Server has been initialized!\n");
		
	}

	// Initializes a client Node
	void client_init(unsigned idx, unsigned port, char *server_addr, unsigned server_port)
	{
		current_node = idx;	// set this node idx to client idx
		init_sock_(port);	// initialize sock address info

		// initialize server node info
		nodes_ = new NodeInfo[1];
		nodes_[0].id = 0;
		nodes_[0].address.sin_family = AF_INET;
		nodes_[0].address.sin_port = htons(server_port);

		if (inet_pton(AF_INET, server_addr, &nodes_[0].address.sin_addr) <= 0)
		{
			assert(false && "Invalid server IP address format");
		}

		// send registration message to server to register itself
		Register* msg = new Register(MsgKind::Register, idx, 0, 42, ip_, port);
		send_m(msg);

		// Receives Directory message from server node
		Directory *ipd = dynamic_cast<Directory *>(recv_m());
		printf("Client has registered!\n");
		

		NodeInfo * nodes = new NodeInfo[num_nodes];
		nodes[0] = nodes_[0];

		for (size_t i = 0; i < num_nodes; i++)
		{
			nodes[i+1].id = i + 1;
			nodes[i+1].address.sin_family = AF_INET;
			nodes[i+1].address.sin_port = htons(ipd->ports_->get(i));
			if (inet_pton(AF_INET, ipd->addresses_->get(i)->c_str(), &nodes_[i+1].address.sin_addr) <= 0)
			{
				printf("Invalid IP directory-addr for node %zu", (i + 1));
				exit(1);
			}
		}
		delete [] nodes_;
		nodes_ = nodes;	// replace provisional nodes with the new nodes
		delete ipd;
	}

	// sends a message to another node
	void send_m(Message *msg) override
	{
		NodeInfo &target = nodes_[msg->getTarget()];

		int connected = socket(AF_INET, SOCK_STREAM, 0);
		assert(connected >= 0 && "Unable to create client socket");
		if (connect(connected, (sockaddr *)&target.address, sizeof(target.address)) < 0)
		{
			printf("Unable to connect to remote node :(\n");
			perror("Unable to connect to remote node :(");
		}
		// serialize the message
		Serializer* ser = new Serializer();
		msg->serialize(ser);
		char *buffer = ser->getSerChar();
		size_t size = ser->getPos();
		// send the serialized message
		int status = send(connected, &size, sizeof(size_t), 0);
		assert(status >= 0);
		int status2 = send(connected, buffer, size, 0);
		assert(status2 >= 0);
		printf("sent!\n");
	}


	// node receives a message
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
		size_t rd = 0;
		// reads the serialized message
		while (rd != size)
		{
			rd += read(req, buffer + rd, size - rd);
		}

		// deserializes the message
		Deserializer* des = new Deserializer(buffer);
		Message *msg = Message::deserializeMsg(des);
		printf("msg kind is %i\n", (int)msg->kind_);
		return msg;
	}

};
