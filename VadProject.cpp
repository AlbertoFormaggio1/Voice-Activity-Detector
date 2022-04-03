#include <vector>
#include <fstream>      // std::ifstream
#include <iostream>
#include "Packet.h"
#include "Vad.h"

//The size of each packet
const int packet_size = 160;

using namespace std;

int main()
{
	int FILE_NUMBER;
	cout << "Write the number of the file you want to open N: ";
	cin >> FILE_NUMBER;
	while(cin.fail()) {
        cout << "Error: you have to write a number" << std::endl;
        cin.clear();
        cin.ignore(256,'\n');
		cout << "Write the number of the file you want to open N: ";
        cin >> FILE_NUMBER;
    }
	
    //Open the file for both the input and output stream
    string in_filename = "./inputdatabase/inputaudio" + to_string(FILE_NUMBER) + ".data";
    string out_filename = "outputVADN" + to_string(FILE_NUMBER) + ".data";
    string out_binary_filename = "outputVADN" + to_string(FILE_NUMBER) + ".txt";

    ifstream infile(in_filename, std::ifstream::binary);
	if(!infile.good())
	{
		cout << "There is no file ./inputdatabase/inputaudio" + to_string(FILE_NUMBER) + ".data. Check either the number or the relative path" ;
		return -1;
	}
	
    ofstream outfile(out_filename, std::ofstream::binary);
    ofstream outfile_binary(out_binary_filename, std::ofstream::binary);

    //Creating the buffer for reading bytes and the detector
    char* buffer = new char[packet_size];
    VAD detector = VAD(packet_size);
    
    //Iterates until the end of file is not reached
    while (infile.good()) {   
        int n;
        // read data as a block:
        infile.read(buffer, packet_size);

        //number of bytes read. The last packet can be less than the actual packet size (PACKET_SIZE)
        n = infile.gcount();

        //Add packet to the detector
        detector.add_packet(buffer,n);

        //get the data that were sent by the VAD algorithm, if any
        vector<char> sent_data = detector.get_sent_data();

        //Write the bytes that were sent on the file
        outfile.write((char*)(sent_data.data()), sent_data.size());

        //Print the binary data (0/1) on the standard output
        vector<bool> boolean = detector.get_boolean_result();
        for (bool b : boolean)
        {
            cout << b;
            outfile_binary << b;
        }        
    }

    //Write the last bytes that were not sent earlier
    detector.finalize();
    vector<char> sent_data = detector.get_sent_data();
    outfile.write((char*)(sent_data.data()), sent_data.size());

    //Close the open streams and release the allocated space for the buffer
    delete[] buffer;
    infile.close();
    outfile.close();

    return 0;
}
