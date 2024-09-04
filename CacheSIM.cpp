#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <bitset>
#include <sstream>
using namespace std;
class Cache {
public:
    int ways; // Number of ways
    int cache_size; // Cache size in kilobytes
    int block_size; // Block size in bytes
    double hit_rate;
    double miss_rate;
    long long int hits;
    long long int misses;
    int indices;
    int index_bits;
    int offset_bits;
    int tag_bits;
    vector<vector<vector<string>>> cache_mem;
    vector<string> traces;

    Cache(int ways1, int cache_size1, int block_size1)
        : ways(ways1), cache_size(cache_size1), block_size(block_size1) {
        traces = {
            "C:\\IIITB SEM 3\\CA assignment\\traces\\gcc.trace",
            "C:\\IIITB SEM 3\\CA assignment\\traces\\gzip.trace",
            "C:\\IIITB SEM 3\\CA assignment\\traces\\mcf.trace",
            "C:\\IIITB SEM 3\\CA assignment\\traces\\swim.trace",
            "C:\\IIITB SEM 3\\CA assignment\\traces\\twolf.trace"
        };

        hits = 0;
        misses = 0;
        hit_rate = 0.0;
        miss_rate = 0.0;

        indices = (cache_size * 1024) / (block_size * ways);
        index_bits = log2(indices);
        offset_bits = log2(block_size);
        tag_bits = 32 - index_bits - offset_bits;

        //cache_mem.resize(ways, vector<vector<string>>(indices, vector<string>(3, "0")));
    }
    string hexTo32BitBinary(const string& hexStr) {
        // Convert the hex string to an integer
        unsigned int value;
        stringstream ss;
        ss << std::hex << hexStr;
        ss >> value;

        // Use std::bitset to convert the integer to a 32-bit binary string
        bitset<32> binary(value);

        // Return the binary string representation
        return binary.to_string();
    }
    void calculateRates() {
        int fileNo = 0;
        vector<string> FileName = {"gcc.trace", "gzip.trace", "mcf.trace", "swim.trace", "twolf.trace"};
        
        for (const auto& trace_iter : traces) {
            hits=0;
            misses=0;
            cache_mem.resize(ways, vector<vector<string>>(indices, vector<string>(3, "0")));
            ifstream inputFile(trace_iter);
            if (!inputFile) {
                cerr << "Error opening file: " << trace_iter << endl;
                continue;
            }

            string line;
            while (getline(inputFile, line)) {
                line = line.substr(4, 8);                                           // Extract the address part
                string address = hexTo32BitBinary(line);                            //convert to a binary string 

                string Gtag = address.substr(0, tag_bits);                          //slices the address to get tag
                int Gindex = stoi(address.substr(tag_bits, index_bits), nullptr, 2);//get the index of the address requested

                bool hit = false;                                                   //to see if it is a cache hit or a miss
                //implimentation of a cache hit 
                for (size_t j = 0; j < ways; ++j) {
                    if (cache_mem[j][Gindex][0] == Gtag && cache_mem[j][Gindex][1] == "1") {
                        hits++;
                        hit = true;
                        for (size_t k = 0; k < ways; ++k) {                         //increase the LRU value for all other blocks
                            int temp = stoi(cache_mem[k][Gindex][2]);
                            temp++;
                            cache_mem[k][Gindex][2] = to_string(temp);
                        }
                        break;
                    }
                }
                //implementation of a cache miss 
                if (!hit) {
                    misses++;
                    int max_lru = -1;
                    int max_lru_way = -1;                                           //finding the way which will be replaced 
                    for (size_t i = 0; i < ways; ++i) {
                        int temp = stoi(cache_mem[i][Gindex][2]);
                        if (temp > max_lru) {
                            max_lru = temp;
                            max_lru_way = i;
                        }
                    }
                    cache_mem[max_lru_way][Gindex][0] = Gtag;                       //replacement of the tag
                    cache_mem[max_lru_way][Gindex][1] = "1";                        //set valid bit to 1
                    cache_mem[max_lru_way][Gindex][2] = "0";                        //set LRU to zero as it is just added 

                    for (size_t i = 0; i < ways; ++i) {                             //increase the value of the LRU of the other blocks
                        if (i != max_lru_way) {
                            int temp = stoi(cache_mem[i][Gindex][2]);
                            temp++;
                            cache_mem[i][Gindex][2] = to_string(temp);
                        }
                    }
                }
            }
            inputFile.close();
            
            double total_instructions_double = static_cast<double>(hits + misses);
            hit_rate = static_cast<double>(hits) / total_instructions_double;
            miss_rate = static_cast<double>(misses) / total_instructions_double;
            double hm = static_cast<double>(hits )/static_cast<double>(misses) ;

            cout<<"File name :"<<FileName[fileNo]<<endl;
            cout << "Total hits: "  << " is: " << hits << endl;
            cout << "Total misses: "  << " is: " << misses << endl;
            cout << "Hit Rate (as percent) of file "  << " is: " << hit_rate * 100 << endl;
            cout << "Miss Rate (as percent) of file "  << " is: " << miss_rate * 100 << endl;
            cout<<"Hits\\Miss: "<<hm<<endl;
            cout << "___________________________________________________________________________________________________________________\n";
            fileNo++;
        }
    }
};

int main() {
    int ways; // Number of ways
    int cache_size; // Cache size in kilobytes
    int block_size; // Block size in bytes

    cout << "Enter the number of ways:\n";
    cin >> ways;
    cout << "Enter the cache size in kilobytes:\n";
    cin >> cache_size;
    cout << "Enter the block size in bytes:\n";
    cin >> block_size;

    Cache L1(ways, cache_size, block_size);
    L1.calculateRates();

    return 0;
}
