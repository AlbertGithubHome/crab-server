#ifndef __I_NET_RING_BUFFER_H__
#define __I_NET_RING_BUFFER_H__

class i_net_ring_buffer
{
public:
    i_net_ring_buffer() {}
    virtual ~i_net_ring_buffer() {}

public:
    // memory space size == buffer_len + max_pack + 6 => |buffer_len|interval=4|tmp_len=2|max_pack|
    virtual bool init(const int socket_id, const unsigned int buffer_len, const unsigned int max_pack) = 0;
    virtual bool reinit(const int socket_id) = 0;
    virtual bool put_pack(const void* data, const unsigned short data_len) = 0;
    virtual const void* get_pack(unsigned short& data_len) = 0;
    virtual bool recv_date() = 0;
    virtual bool send_data() = 0;
    virtual void release() = 0;
};

i_net_ring_buffer* create_net_ring_buffer();

#endif//__I_NET_RING_BUFFER_H__