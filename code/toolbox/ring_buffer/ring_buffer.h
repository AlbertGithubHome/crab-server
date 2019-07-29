#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__
#include "i_ring_buffer.h"

class ring_buffer : public i_ring_buffer
{
public:
    ring_buffer();
    ~ring_buffer();

public:
    bool init(const unsigned int buffer_len, const unsigned int max_pack);
    bool put_pack(const void* data, const unsigned int data_len);
    const void* get_pack(unsigned int& data_len);
    void release();

private:
    char* put_date(const char* read_ptr, char* tmp_write_ptr, const char* data, const unsigned int data_len);
    const unsigned int get_date_len(const char* read_ptr, const char* write_ptr);

private:
    unsigned int m_buff_len;
    unsigned int m_max_pack;

    char* m_head_ptr;   // head pointer
    char* m_read_ptr;   // read pointer
    char* m_writ_ptr;   // write pointer

    char* m_next_ptr;   // next read pointer
    char* m_temp_ptr;   // temp pointer for wraping
};

#endif//__RING_BUFFER_H__