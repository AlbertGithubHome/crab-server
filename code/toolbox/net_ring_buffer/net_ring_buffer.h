#ifndef __NET_RING_BUFFER_H__
#define __NET_RING_BUFFER_H__
#include "i_net_ring_buffer.h"

class net_ring_buffer : public i_net_ring_buffer
{
public:
    net_ring_buffer();
    ~net_ring_buffer();

public:
    bool init(const int socket_id, const unsigned int buffer_len, const unsigned int max_pack);
    bool reinit(const int socket_id);

    bool put_pack(const void* data, const unsigned short data_len);
    const void* get_pack(unsigned short& data_len);

    bool recv_date();
    bool send_data();

    void release();

private:
    char* put_date(const char* read_ptr, char* tmp_write_ptr, const char* data, const unsigned short data_len);
    const unsigned short get_date_len(const char* read_ptr, const char* write_ptr);

private:
    int m_socket_id;

    unsigned int m_buff_len;
    unsigned int m_max_pack;

    char* m_head_ptr;   // head pointer
    char* m_read_ptr;   // read pointer
    char* m_writ_ptr;   // write pointer

    char* m_next_ptr;   // next read pointer
    char* m_temp_ptr;   // temp pointer for wraping
};

#endif//__NET_RING_BUFFER_H__