#ifndef HEADER_LOGGER_721664FB_4081_41D8_A8C5_98A4754DDCD8
#define HEADER_LOGGER_721664FB_4081_41D8_A8C5_98A4754DDCD8


int  logger_start(char const * const path);
void logger_stop();
void logger_log(const char *__restrict __format, ...);


#endif // HEADER_LOGGER_721664FB_4081_41D8_A8C5_98A4754DDCD8
