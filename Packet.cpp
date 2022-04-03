#include  "Packet.h"

Packet::Packet()
{
    raw_data = std::vector<char>();
    c_packet = std::vector<Complex>();
}

Packet::Packet(int samples_num) : Packet()
{
    for (int i = 0; i < samples_num; i++)
    {
        raw_data.push_back(0);
        //Create a new Complex number with Re = 0, Im = 0
        c_packet.push_back(Complex(0, 0));
    }

    size = samples_num;
}

Packet::Packet(char* buf, int samples_num) : Packet()
{
    for (int i = 0; i < samples_num; i++)
    {
        char el = buf[i];
        //Insert the raw data inside the vector
        raw_data.push_back(el);
        //Create a new Complex number with Re = el, Im = 0
        c_packet.push_back(Complex((double)el, 0));
    }

    size = samples_num;
}