#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#ifdef WIN32
#include <Windows.h>
//#include <WinSock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "net_ring_buffer.h"

#define INTERVAL_SPACE  (4)
#define DATA_LEN_SIZE   (sizeof(unsigned short))

i_net_ring_buffer* create_ring_buffer()
{
    return new net_ring_buffer;
}

net_ring_buffer::net_ring_buffer()
{
    m_socket_id = -1;

    m_buff_len = 0;
    m_max_pack = 0;

    m_head_ptr = NULL;
    m_read_ptr = NULL;
    m_writ_ptr = NULL;
    m_next_ptr = NULL;
    m_temp_ptr = NULL;
}

net_ring_buffer::~net_ring_buffer()
{
    release();
}

bool net_ring_buffer::init(const int socket_id, const unsigned int buffer_len, const unsigned int max_pack)
{
    if (buffer_len < max_pack + DATA_LEN_SIZE || max_pack == 0)
        return false;

    m_head_ptr = new char[buffer_len + INTERVAL_SPACE + DATA_LEN_SIZE + max_pack];
    if (NULL == m_head_ptr)
        return false;

    m_socket_id = socket_id;

    m_buff_len = buffer_len;
    m_max_pack = max_pack;

    m_read_ptr = m_head_ptr;
    m_writ_ptr = m_head_ptr;
    m_next_ptr = m_head_ptr;

    m_temp_ptr = m_head_ptr + buffer_len + INTERVAL_SPACE;

    return true;
}

bool net_ring_buffer::reinit(const int socket_id)
{
    if (NULL == m_head_ptr)
        return false;

    m_socket_id = socket_id;

    m_read_ptr = m_head_ptr;
    m_writ_ptr = m_head_ptr;
    m_next_ptr = m_head_ptr;

    m_temp_ptr = m_head_ptr + m_buff_len + INTERVAL_SPACE;

    return true;
}

bool net_ring_buffer::put_pack(const void* data, const unsigned short data_len)
{
    if (NULL == data || data_len == 0)
        return false;

    if (NULL == m_head_ptr || data_len > m_max_pack)
        return false;

    char* writ_ptr = m_writ_ptr;
    const char* read_ptr = m_read_ptr;

    unsigned int content_len = writ_ptr >= read_ptr ? writ_ptr - read_ptr : m_buff_len - (read_ptr - writ_ptr);
    unsigned int remains_len = m_buff_len - content_len - 1; // need a byte space to judge full or empty

    // space is not enough
    if (data_len + DATA_LEN_SIZE > remains_len)
        return false;

    // write length
    writ_ptr = put_date(read_ptr, writ_ptr, (char*)&data_len, DATA_LEN_SIZE);
    if (NULL == writ_ptr)
        return false;

    // write data
    writ_ptr = put_date(read_ptr, writ_ptr, (char*)data, data_len);
    if (NULL == writ_ptr)
        return false;

    m_writ_ptr = writ_ptr;

    return true;
}

const void* net_ring_buffer::get_pack(unsigned short& data_len)
{
    data_len = 0;
    if (NULL == m_head_ptr)
        return NULL;

    // ensure that this position cant be write
    m_read_ptr = m_next_ptr;

    const char* read_ptr = m_read_ptr;
    const char* writ_ptr = m_writ_ptr;
    if (read_ptr == m_writ_ptr)
        return NULL;

    unsigned int content_len = writ_ptr >= read_ptr ? writ_ptr - read_ptr : m_buff_len - (read_ptr - writ_ptr);
    if (content_len <= DATA_LEN_SIZE)
        return NULL;

    data_len = get_date_len(read_ptr, writ_ptr);

    // not read complete
    if (content_len < data_len + DATA_LEN_SIZE)
        return NULL;

    const unsigned int read_right_remain = m_buff_len - (read_ptr - m_head_ptr);
    const unsigned int data_total_len = data_len + DATA_LEN_SIZE;

    if (read_right_remain >= data_total_len)
    {
        m_next_ptr = read_right_remain == data_total_len ? m_head_ptr : m_next_ptr + data_total_len;
        return read_ptr + DATA_LEN_SIZE;
    }

    // wrap and copy data to temp_ptr and return temp_ptr
    memcpy(m_temp_ptr, read_ptr, read_right_remain);
    memcpy(m_temp_ptr + read_right_remain, m_head_ptr, data_len - read_right_remain);

    m_next_ptr = m_head_ptr + data_len - read_right_remain;

    return m_temp_ptr + DATA_LEN_SIZE;
}

bool net_ring_buffer::recv_date()
{
    if (m_socket_id < 0)
        return false;

    char* recv_start = m_writ_ptr;
    char* recv_ended = m_read_ptr;

    unsigned int content_len = recv_start >= recv_ended ? recv_start - recv_ended : m_buff_len - (recv_ended - recv_start);
    unsigned int remains_len = m_buff_len - content_len - 1; // need a byte space to judge full or empty

    // buff is full
    if (remains_len <= 0)
        return false;

    // need a byte space to judge full or empty
    unsigned int recv_right_remain = m_buff_len - (recv_start - m_head_ptr) - (recv_ended == m_head_ptr ? 1 : 0);

    unsigned int toread_count = recv_right_remain;
    unsigned int readed_count = 0;

    // recv date to fill in sapce before wrap
    if (recv_start >= recv_ended)
    {
        readed_count = ::recv(m_socket_id, recv_start, toread_count, 0);
        // read all data complete
        if (readed_count > 0 && readed_count < recv_right_remain)
        {
            m_writ_ptr = recv_start + readed_count;
            return true;
        }
        else if (readed_count == toread_count)
        {
            m_writ_ptr = m_head_ptr;
            recv_start = m_head_ptr;

            // buff is full and don't know whether there's any more data to read
            if (remains_len == readed_count)
                return false;
        }
        else if (readed_count == 0 || errno != EAGAIN)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    // need a byte space to judge full or empty
    toread_count = recv_ended - recv_start - 1;
    readed_count = ::recv(m_socket_id, recv_start, toread_count, 0);
    // read all data complete
    if (readed_count > 0 && readed_count <= toread_count)
    {
        m_writ_ptr = recv_start + readed_count;
        // readed_count == toread_count => buff is full and don't know whether there's any more data to read
        return readed_count < toread_count;
    }
    else if (readed_count == 0 || errno != EAGAIN)
    {
        return false;
    }

    return true;
}

bool net_ring_buffer::send_data()
{
    if (m_socket_id < 0)
        return false;

    char* send_start = m_read_ptr;
    char* send_ended = m_writ_ptr;

    unsigned int content_len = send_ended >= send_start ? send_ended - send_start : m_buff_len - (send_start - send_ended);
    // no data to send
    if (content_len <= 0)
        return true;

    // data len before warp
    unsigned int tosend_count = send_ended >= send_start ? send_ended - send_start : m_buff_len - (send_start - m_head_ptr);
    unsigned int sended_count;

    // send date of sapce before wrap firstly
    if (send_start > send_ended)
    {
        sended_count = ::send(m_socket_id, send_start, tosend_count, 0);
        if (sended_count == tosend_count)
        {
            m_read_ptr = m_head_ptr;
            send_start = m_head_ptr;

            if (sended_count == content_len)
                return true;
        }
        else if (sended_count <= 0) // system cache is full
        {
            return true;
        }
        else if (errno != EAGAIN)
        {
            return false;
        }
    }

    tosend_count = send_ended - send_start;
    sended_count = ::send(m_socket_id, send_start, tosend_count, 0);
    if (sended_count <= tosend_count)
    {
        m_read_ptr = send_start + sended_count;
        return true;
    }
    else if (errno != EAGAIN)
    {
        return false;
    }

    return true;
}

void net_ring_buffer::release()
{
    if (NULL != m_head_ptr)
        delete[] m_head_ptr;

    m_socket_id = -1;

    m_buff_len = 0;
    m_max_pack = 0;

    m_head_ptr = NULL;
    m_read_ptr = NULL;
    m_writ_ptr = NULL;
    m_next_ptr = NULL;
    m_temp_ptr = NULL;
}

char* net_ring_buffer::put_date(const char* read_ptr, char* tmp_write_ptr, const char* data, const unsigned short data_len)
{
    const unsigned int write_right_remain = m_buff_len - (tmp_write_ptr - m_head_ptr);
    if (write_right_remain >= data_len)
    {
        memcpy(tmp_write_ptr, data, data_len);
        return write_right_remain == data_len ? m_head_ptr : tmp_write_ptr + data_len;
    }

    // wrap
    memcpy(tmp_write_ptr, data, write_right_remain);
    memcpy(m_head_ptr, data + write_right_remain, data_len - write_right_remain);

    return m_head_ptr + data_len - write_right_remain;
}

const unsigned short net_ring_buffer::get_date_len(const char* read_ptr, const char* write_ptr)
{
    const unsigned int write_right_remain = m_buff_len - (write_ptr - m_head_ptr);
    if (write_right_remain >= DATA_LEN_SIZE)
    {
        return *((unsigned int *)read_ptr);
    }

    // wrap
    unsigned short data_len = 0;
    char* data = (char*)&data_len;
    memcpy(data, read_ptr, write_right_remain);
    memcpy(data + write_right_remain, m_head_ptr, DATA_LEN_SIZE - write_right_remain);
    return data_len;
}

