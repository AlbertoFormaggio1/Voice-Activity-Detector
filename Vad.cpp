#define _USE_MATH_DEFINES
#include "Vad.h"
#include <cmath>
#include <algorithm>

VAD::VAD(int packet_size) : PACKET_SIZE(packet_size){
    sample_start = 0;
    result = std::vector<char>();
}

std::vector<Complex> VAD::FFT(std::vector<Complex> input)
{
    int N = input.size();
    if (N == 1)
        return input;

    //even elements
    std::vector<Complex> evenVec = std::vector<Complex>();
    for (int i = 0; i < N / 2; i++)
    {
        Complex c = Complex(input[2 * i]);
        evenVec.push_back(c);
    }
    //Recursively call Fast Fourier transform for the even elements 
    evenVec = FFT(evenVec);

    std::vector<Complex> oddVec = std::vector<Complex>();
    for (int i = 0; i < N / 2; i++)
    {
        Complex c = Complex(input[(2 * i) + 1]);
        oddVec.push_back(input[(2 * i) + 1]);
    }
    //Recursively call Fast fourier transform for odd elements
    oddVec = FFT(oddVec);

    //Result
    std::vector<Complex> result = std::vector<Complex>(N);

    for (int i = 0; i < (N / 2); i++)
    {
        //Evaluate omega (pulse frequency)
        double w = (2.0 * i * M_PI) / N;
        //Create imaginary number corresponding to that omega
        Complex wk = Complex(cos(w), sin(w));
        Complex even = evenVec[i];
        Complex odd = oddVec[i];

        result[i] = even + (wk * odd);
        result[i + (N / 2)] = even - (wk * odd);
    }
    return result;
}


//Current has the value previous to the one we are considering at the moment. We are updating current
//with packet.
void VAD::add_packet(char* buffer, int size)
{
    //Move all the packets back of one position in order to leave current free, this way it can host the new value
    prevPrevPrev = prevPrev;
    prevPrev = prev;
    prev = current;
    current = Packet(buffer, size);

    //Detects if the given packet is a voice packet or not
    detect_activity(current);

    //If this is the first data packet, then send it if it contains voice, otherwise do not do anything
    if (sample_start == 0)
    {
        if (current.is_voice())
        {
            send(current);
            boolean_result.push_back(1);
        }
    }
    else
    {
        //If the third-to-last packet (if existing) was not sent, then it was not near to any packet containing voice. Hence, send a silent
        //packet.
        if (!prevPrevPrev.is_sent() && sample_start >= 3 * PACKET_SIZE)
        {
            send_silent(prevPrevPrev.get_size());
            boolean_result.push_back(0);
        }
        //If  the current packet contains voice
        if (current.is_voice())
        {
            //If the previous packet was not sent (it wasn't neither voice nor near to any packet containing voice) and the current one
            //contains voice, send it.
            if (!prev.is_sent())
            {
                //If the second-to-last packet (if existing) was not sent, and the previous one has to be sent, then send a silent packet
                //Before sending the previous packet
                if (!prevPrev.is_sent() && sample_start >= 2 * PACKET_SIZE)
                {
                    send_silent(prevPrev.get_size());
                    prevPrev.send();
                    boolean_result.push_back(0);
                }
                //Send the previous packet
                send(prev);
                boolean_result.push_back(1);
            }
            //send the current packet
            send(current);
            boolean_result.push_back(1);
        }
        else
        {
            //if the current packet is not voice but any of the last three packets do contain voice, send the current packet.
            //(I am looking at the past. I am sending the current packet even though it was not voice in order to achieve better
            //audio quality in output)
            if (prev.is_voice() || prevPrev.is_voice() || prevPrevPrev.is_voice())
            {
                send(current);
                boolean_result.push_back(1);
            }
        }
    }
    //Move the position inside the stream
    sample_start = sample_start + size;
}

void VAD::finalize()
{
    //If the last packet was not sent, then it was not voice. A silence has to be sent in his place
    if (!current.is_sent())
        send_silent(current.get_size());

    //Same for the previous packets
    if(!prev.is_sent())
        send_silent(prev.get_size());

    if(!prevPrev.is_sent())
        send_silent(prevPrev.get_size());
}

void VAD::send_silent(int size)
{
    //Create a silent packet with all 0s
    Packet silentPacket = Packet(size);
    //Send the packet
    send(silentPacket);
}

void VAD::send(Packet& p)
{
    //Get raw data
    std::vector<char> v = p.get_raw();
    //Insert inside the result vector the values inside the packet that has to be print
    result.insert(result.end(), v.begin(), v.end());
    //set the flag for the packet
    p.send();
}

std::vector<char> VAD::get_sent_data()
{
    //Copy raw data inside vector
    std::vector<char> to_return = result;
    //erase its content
    result.clear();
    return to_return;
}

std::vector<bool> VAD::get_boolean_result()
{
    std::vector<bool> to_return = boolean_result;
    boolean_result.clear();
    return to_return;
}

/*
* Detects the activity of the given packet and sets its properties whether it's voice or not
*/
void VAD::detect_activity(Packet& p)
{
    //Applies the Fast Fourier Transform to the given packet
    std::vector<Complex> fft_p = FFT(p.get_complex());
    //Get the frequencies corresponding to each complex number
    std::vector<double> frequencies = evaluate_frequencies(fft_p);

    //Get the max frequency
    double peak = *max_element(frequencies.begin(), frequencies.end());

    //Set the flag is_voice inside the packet if the packet is inside the frequency range for human voices [200,3400]
    p.set_voice(peak >= MIN_BAND && peak <= MAX_BAND);
}


std::vector<double> VAD::evaluate_frequencies(std::vector<Complex>& data) {
    std::vector<double> frequencies = std::vector<double>();

    for (Complex& c : data)
    {
        //Evaluate the magnitude of the complex number c
        double magnitude = sqrt(pow(c.imag(),2) + pow(c.real(),2));
        frequencies.push_back(magnitude);
    }

    return frequencies;
}