#ifndef DUAL_STREAM_H
#define DUAL_STREAM_H

#include <Stream.h>

class DualStream : public Stream
{
private:
    Stream& primaryStream;
    Stream& secondaryStream;

public:
    DualStream(Stream& primary, Stream& secondary)
        : primaryStream(primary)
          , secondaryStream(secondary)
    {
    }

    int available() override
    {
        return primaryStream.available();
    }

    int read() override
    {
        return primaryStream.read();
    }

    int peek() override
    {
        return primaryStream.peek();
    }

    void flush() override
    {
        primaryStream.flush();
        secondaryStream.flush();
    }

    size_t write(uint8_t data) override
    {
        primaryStream.write(data);
        return secondaryStream.write(data);
    }
};

#endif //DUAL_STREAM_H
