#pragma once
#include <complex>
#include <vector>

typedef std::complex<double> Complex;

class Packet
{
private:
    //Raw data
    std::vector<char> raw_data;
    //Raw data converted into complex numbers
    std::vector<Complex> c_packet;
    //Flag used to state whether the packet is voice or not. Before analizing the packet, the default is FALSE
    bool isvoice = false;
    //Flag used to state whether the packet was sent or not (regardless of the fact it is voice or not)
    bool sent = false;
    //The actual size of the packet (the number of samples)
    int size = 0;

public: 
    /*!
     * @brief Default constructor: intializes the packet
    */
    Packet();

    /*!
     * @brief Creates a new packet with as samples_num zeros in it and converts them into complex numbers for the FFT
     * @param samples_num The number of zeros to insert in the packet
    */
    Packet(int samples_num);

    /*!
     * @brief Creates a new packet with the samples in the given buffer and converts them into complex numbers for the FFT
     * @param buf The buffer containing the samples
     * @param samples_num The number of samples to insert in the packet
    */
    Packet(char* buf, int samples_num);

    /*!
     * @brief Returns the vector with the raw data inside the packet
     * @return vector with raw data
    */
    inline std::vector<char> get_raw() { return raw_data; }

    /*!
     * @brief Returns the vector with the complex numbers corresponding to the raw data
     * @return the vector with complex numbers
    */
    inline std::vector<Complex> get_complex() { return c_packet; }

    inline bool is_voice() { return isvoice; }

    inline void set_voice(bool v) { isvoice = v; }

    inline void send() { sent = true; }

    inline bool is_sent() { return sent; }

    inline int get_size() { return size; }
};