#ifndef __LOG_SYSTEM_H__
#define __LOG_SYSTEM_H__
#include "../../share/i_log_system.h"

#ifndef LOG_FILE_MAX_PATH
#define LOG_FILE_MAX_PATH 512
#endif

class log_system : public i_log_system
{
public:
    log_system();
    ~log_system();

public:
    void init_dir(const char* dir);
    void write_log(const e_log_level lv, const char *log_msg, ...);

    void output_console_toggle(const bool on);
    void output_file_toggle(const bool on);

    void set_log_level(const e_log_level lv);
    void set_linefeed(const bool on);

private:
    bool is_valid_level(const e_log_level lv);

    void write2file(const char* msg);
    void write2console(const e_log_level lv, const char* msg);

private:
    e_log_level m_log_lv;
    char        m_root_dir[LOG_FILE_MAX_PATH];
    bool        m_file_toggle;
    bool        m_console_toggle;
    bool        m_linefeed;
};
#endif//__LOG_SYSTEM_H__