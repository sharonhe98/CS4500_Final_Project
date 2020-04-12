#pragma once
#include "message.h"
#include "../object.h"

class NetworkIfc : public Object {
	public:
		/** Register the node with the given node index */
		virtual void register_node(size_t idx) {}
		/** Return the index of the node */
		virtual size_t index() { assert(false); }
		/** Send the message msg; msg is consumed (i.e. deleted) */
		virtual void send_m(Message* msg) = 0;
		/** Wait for a message to arrive. The message becomes owned. */
		virtual Message* recv_m() = 0;
};
