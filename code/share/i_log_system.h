#ifndef __I_LOG_SYSTEM_H__
#define __I_LOG_SYSTEM_H__

// max size for a log msg
#define LOG_MSG_MAXSIZE (4 * 1024)

enum e_log_level
{
    log_lv_infos = 0,
    log_lv_debug = 1,
    log_lv_warns = 2,
    log_lv_error = 3,
    log_lv_fatal = 4,
    log_lv_top,
};

class i_log_system
{
public:
    i_log_system() {}
    virtual ~i_log_system() {}

public:
    virtual void init_dir(const char* dir) = 0;
    virtual void write_log(const e_log_level lv, const char *log_msg, ...) = 0;

    virtual void output_console_toggle(const bool on) = 0;
    virtual void output_file_toggle(const bool on) = 0;

    virtual void set_log_level(const e_log_level lv) = 0;
    virtual void set_linefeed(const bool on) = 0;
};
extern i_log_system& log;

#endif//__I_LOG_SYSTEM_H__