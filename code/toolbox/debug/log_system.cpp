#include "log_system.h"
#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#endif
#pragma warning(disable : 4996)


log_system log_object;
i_log_system& log = log_object;

static void get_current_time_str(char *time_str)
{
    if (time_str == NULL)
        return;

    time_t	current_time;
    time(&current_time);
    tm* cur_time_struct = localtime(&current_time);

    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d",
        cur_time_struct->tm_year + 1900,
        cur_time_struct->tm_mon + 1,
        cur_time_struct->tm_mday,
        cur_time_struct->tm_hour,
        cur_time_struct->tm_min,
        cur_time_struct->tm_sec);
}

static const char* get_log_lv_str(const e_log_level lv)
{
    switch (lv)
    {
    case log_lv_debug:
        return " [debug] ";
        break;
    case log_lv_warns:
        return " [warns] ";
        break;
    case log_lv_error:
        return " [error] ";
        break;
    case log_lv_fatal:
        return " [fatal] ";
        break;
    default:
        return " [infos] ";
        break;
    }
}

log_system::log_system()
{
    m_log_lv = log_lv_infos;
    m_console_toggle = true;
    m_file_toggle = true;
    m_linefeed = true;
    strcpy(m_root_dir, "./");
}

log_system::~log_system()
{

}

void log_system::init_dir(const char* dir)
{
    strncpy(m_root_dir, dir, LOG_FILE_MAX_PATH - 1);
}

void log_system::write_log(const e_log_level lv, const char *log_msg, ...)
{
    if (!m_console_toggle && !m_file_toggle)
        return;

    if (!is_valid_level(lv))
        return;

    static char msg[LOG_MSG_MAXSIZE] = { 0 };
    static char output_msg[LOG_MSG_MAXSIZE] = { 0 };
    va_list va;
    va_start(va, log_msg);
    vsprintf(msg, log_msg, va);

    get_current_time_str(output_msg);
    strcat(output_msg, get_log_lv_str(lv));
    strcat(output_msg, msg);

    if (m_linefeed)
        strcat(output_msg, "\r\n");

    if (m_console_toggle) write2console(lv, output_msg);

    if (m_file_toggle) write2file(lv, output_msg);
}

void log_system::output_console_toggle(const bool on)
{
    m_console_toggle = on;
}

void log_system::output_file_toggle(const bool on)
{
    m_file_toggle = on;
}

void log_system::set_log_level(const e_log_level lv)
{
    m_log_lv = lv;
}

void log_system::set_linefeed(const bool on)
{
    m_linefeed = on;
}

bool log_system::is_valid_level(const e_log_level lv)
{
    return lv >= log_lv_infos && lv < log_lv_top;
}

void log_system::write2file(const e_log_level lv, const char* msg)
{

}

void log_system::write2console(const e_log_level lv, const char* msg)
{
    if (m_log_lv > lv)
        return;

#ifdef WIN32
    unsigned short color = 0;
    switch (lv)
    {
    case log_lv_debug:
        color = FOREGROUND_GREEN;//green
        break;
    case log_lv_warns:
        color = FOREGROUND_RED | FOREGROUND_GREEN;//yellow
        break;
    case log_lv_error:
        color = FOREGROUND_RED;//red
        break;
    case log_lv_fatal:
        color = FOREGROUND_RED | FOREGROUND_BLUE;//magenta
        break;
    default:
        color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;//white
        break;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);

    printf(msg);
#endif
}

