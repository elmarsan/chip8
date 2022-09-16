#ifndef CHIP8_LOGGER_H
#define CHIP8_LOGGER_H

#ifdef LOGGER
#define LDEBUG(...) printf(__VA_ARGS__)
#else
#define LDEBUG(...)
#endif

#endif //CHIP8_LOGGER_H
