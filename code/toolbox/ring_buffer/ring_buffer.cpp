#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ring_buffer.h"

#define INTERVAL_SPACE  (4)
#define DATA_LEN_SIZE   (sizeof(unsigned int))

i_ring_buffer* create_ring_buffer()
{
    return new ring_buffer;
}

ring_buffer::ring_buffer()
{
    m_buff_len = 0;
    m_max_pack = 0;

    m_head_ptr = NULL;
    m_read_ptr = NULL;
    m_writ_ptr = NULL;
    m_next_ptr = NULL;
    m_temp_ptr = NULL;
}

ring_buffer::~ring_buffer()
{
    release();
}

bool ring_buffer::init(const unsigned int buffer_len, const unsigned int max_pack)
{
    if (buffer_len < max_pack + DATA_LEN_SIZE || max_pack == 0)
        return false;

    m_head_ptr = new char[buffer_len + INTERVAL_SPACE + DATA_LEN_SIZE + max_pack];
    if (NULL == m_head_ptr)
        return false;

    m_buff_len = buffer_len;
    m_max_pack = max_pack;

    m_read_ptr = m_head_ptr;
    m_writ_ptr = m_head_ptr;
    m_next_ptr = m_head_ptr;

    m_temp_ptr = m_head_ptr + buffer_len + INTERVAL_SPACE;

    return true;
}

bool ring_buffer::put_pack(const void* data, const unsigned int data_len)
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

const void* ring_buffer::get_pack(unsigned int& data_len)
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

void ring_buffer::release()
{
    if (NULL != m_head_ptr)
        delete[] m_head_ptr;
    
    m_buff_len = 0;
    m_max_pack = 0;

    m_head_ptr = NULL;
    m_read_ptr = NULL;
    m_writ_ptr = NULL;
    m_next_ptr = NULL;
    m_temp_ptr = NULL;
}

char* ring_buffer::put_date(const char* read_ptr, char* tmp_write_ptr, const char* data, const unsigned int data_len)
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

const unsigned int ring_buffer::get_date_len(const char* read_ptr, const char* write_ptr)
{
    const unsigned int write_right_remain = m_buff_len - (write_ptr - m_head_ptr);
    if (write_right_remain >= DATA_LEN_SIZE)
    {
        return *((unsigned int *)read_ptr);
    }

    // wrap
    unsigned int data_len = 0;
    char* data = (char*)&data_len;
    memcpy(data, read_ptr, write_right_remain);
    memcpy(data + write_right_remain, m_head_ptr, DATA_LEN_SIZE - write_right_remain);
    return data_len;
}

