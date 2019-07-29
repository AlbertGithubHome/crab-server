#ifndef __I_RING_BUFFER_H__
#define __I_RING_BUFFER_H__

class i_ring_buffer
{
public:
    i_ring_buffer() {}
    virtual ~i_ring_buffer() {}

public:
    // memory space size == buffer_len + max_pack + 8 => |buffer_len|interval=4|tmp_len=4|max_pack|
    virtual bool init(const unsigned int buffer_len, const unsigned int max_pack) = 0;
    virtual bool put_pack(const void* data, const unsigned int data_len) = 0;
    virtual const void* get_pack(unsigned int& data_len) = 0;
    virtual void release() = 0;
};

i_ring_buffer* create_ring_buffer();

#endif//__I_RING_BUFFER_H__