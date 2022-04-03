#pragma once
#include "Packet.h"

class VAD
{
private:
    int sample_start; //Campione iniziale del pacchetto in considerazione
    int sample_end;
    Packet current;
    Packet prev;
    Packet prevPrev;
    Packet prevPrevPrev;
    //Frequenza minima per essere voce
    static constexpr int MIN_BAND = 200;
    //Frequenza massima per essere voce
    static constexpr int MAX_BAND = 3400;
    const int PACKET_SIZE;

    std::vector<char> result;
    std::vector<bool> boolean_result;

    /*
    * Detects the activity of the given packet and sets its properties whether it's voice or not
    */
    static void detect_activity(Packet& p);

    /*!
     * @brief Evaluates the frequencies corresponding to every complex number in the given vector by applying the FFT
     * @param data A vector with the complex numbers (obtained by using the raw data)
     * @return A vector with the frequencies
    */
    static std::vector<double> evaluate_frequencies(std::vector<Complex>& data);

    /*!
     * @brief Evaluates the FFT of the given vector and returns it
    */
    static std::vector<Complex> FFT(std::vector<Complex> input);

    /*!
     * @brief Sends a silent packet (a packet with all 0s) of the given size
     * @param size size of the packet
    */
    void send_silent(int size);

    /*!
     * @brief Sends the given packet
     * @param p The packet to send
    */
    void send(Packet& p);

public:
    /*!
     * @brief Creates a new Voice Activity Detector able to manage any packet with size equal to packet_size
     * @param packet_size 
    */
    VAD(int packet_size);

    /*!
     * @brief Adds the packet inside the buffer to the voice activity detector. Packets can be added to the result vector 
     * if the current packet is voice.
     * 
     * 
     * !!! The decision whether to send packet or silence is made based on THREE PAST PACKETS and on ONE FUTURE PACKET !!!
     * 
     * 
     * After each call of add_packet(), get_sent_data() should be called to obtain the packets that have been sent during that specific call
     * (if you want to get the data in real time)
     * @param buffer The samples of the current packet to be added
     * @param size The size of the current packet
    */
    void add_packet(char* buffer, int size);    

    /*!
     * @brief Finalizes the detection activity. If the last packet of the stream was not sent because it was waiting for the futute one
     (it may happen when none of the last packets contains voice), then a silent packet is sent.
    */
    void finalize();    

    /*!
     * @brief Returns the sent data up until now and deletes anything inside the buffer of the Voice activity detector. You can either call
     * the packet after each call of add_packet in order to obtain the sent packets in real time or at the end of the detection in order
     * to get the data all together.
    */
    std::vector<char> get_sent_data();

    std::vector<bool> get_boolean_result();
};