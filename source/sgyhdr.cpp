#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <cmath>
#include <iomanip>
#include <limits>
#include <unordered_map>
#include <boost/program_options.hpp>
#include "segy.h"
#include "stats.h"
#include "utils.h"

namespace po = boost::program_options;

int DEFAULT_PRECISION = 2;
const std::vector<std::pair<int, int>> DEFAULT_HEADERS = {
        {73, 1},
        {77, 1},
        {81, 1},
        {85, 1}

    };
const int DEFAULT_COL_WIDTH = 12;

int main(int argc, char* argv[]) { 

    po::options_description desc("Allowed options");
    desc.add_options()            
        ("binary,b", "print file's binary header")
        ("traces,t", po::value<std::string>(), "specific traces mode")
        ("stats,s", "all traces statistics mode")
        ("headers,h", po::value<std::string>(), "headers to scan. Format: <B1>:<F1>,<B2>:<F2>,... where <Bn> is starting byte,\
    <Fn> is number format. 0-int16,1-int32,2-IEEE Float,3-IBM Float")     
        ("filename", po::value<std::string>(), "input file name");

    po::positional_options_description p;
    p.add("filename", 1);

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);
    }
    catch (const po::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    if (argc == 1 || vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("filename")) {
        std::string filename = vm["filename"].as<std::string>();

        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return -1;
        }
      
        // Get the file size
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // Skip the first 3200 bytes (text header)
        file.seekg(3200, std::ios::beg);

        // Read the binary header
        binaryFileHeader binaryHeader;
        file.read(reinterpret_cast<char*>(&binaryHeader), sizeof(binaryFileHeader));        

        // Check if the read was successful
        if (!file.good()) {
            std::cerr << "Error reading binary header from file: " << filename << std::endl;
            return 1;
        }

        bool need_swap = false;
        if ((binaryHeader.SAMPLE_FORMAT < 1) | (binaryHeader.SAMPLE_FORMAT > 16)) {
            need_swap = true;            
            binHeaderSwapEndian(binaryHeader);            
        }

        int sample_interval = binaryHeader.INTERVAL_MS;
        int num_samples = binaryHeader.NUM_OF_SAMPLES;

        std::streamsize traceSize = num_samples * sizeof(float) + 240;
        unsigned long numberOfTraces = (fileSize - 3600) / traceSize;           
        
        std::cout << std::string(80, '#') << std::endl;
        std::cout << "File " << filename << ". Basic SEG-Y File Info:" << std::endl;
        if (need_swap) std::cout << "Big Endian Byte Order" << std::endl;
        else std::cout << "Little Endian Byte Order" << std::endl;      
        std::cout << "Number of Traces = " << numberOfTraces << std::endl;
        std::cout << "Sample Interval =  " << sample_interval << std::endl;
        std::cout << "Number of Samples = " << num_samples << std::endl;
        std::cout << std::string(80, '#') << std::endl;

        if (vm.count("binary")) {
            std::cout << std::string(80, '#') << std::endl;
            std::cout << "File " << filename << ". BINARY HEADER" << std::endl;
            std::cout << std::string(80, '#') << std::endl;
            printBinaryHeader(binaryHeader);
        }
        
        if (vm.count("traces") | vm.count("stats")) {
           // std::cout << std::string(80, '#') << std::endl;
            std::cout << "TRACE HEADERS.B-starting byte,F-format(0-int16,1-int32,2-IEEE float,3-IBM float)" << std::endl;
            std::cout << std::string(80, '#') << std::endl;
        }
      
        std::vector<int> numTraces;
        if (vm.count("traces")) numTraces = parse_number_params(vm["traces"].as<std::string>());           
        std::string headers;
        std::vector<std::pair<int, int>> bytes_formats;
      
        if (vm.count("headers")) {
                headers = vm["headers"].as<std::string>();
                bytes_formats = parseHdrParams(headers);
            }
        else bytes_formats = DEFAULT_HEADERS;
        if (numTraces.size() > 0) {             

            std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << "TraceNo" << " ";            

            for (int j = 0; j < bytes_formats.size(); j++) {
                std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << std::right << "B" + std::to_string(bytes_formats[j].first)+\
                  ":F" + std::to_string(bytes_formats[j].second)  << " ";
            }
            std::cout << std::endl;

            unsigned long i = 0;
            do {               
                traceHeader TH;
                std::streamoff offset = 3600 + (numTraces[i] - 1) * (240 + num_samples * 4);
                file.seekg(offset, std::ios::beg);
                file.read(reinterpret_cast<char*>(&TH), sizeof(traceHeader));
                if (need_swap) traceHeaderSwapEndian(TH);

                std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << numTraces[i] << " ";
                for (int j = 0; j < bytes_formats.size(); j ++) {                   
                    float val = getSpecifiedTraceHeaderValue(TH, bytes_formats[j].first, bytes_formats[j].second);
                    std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << (int)val << " ";
                }
                std::cout << std::endl;
                ++i;              
            }
            while (i < numTraces.size() && numTraces[i] < numberOfTraces);           
        }
        else {
            std::vector<Statistics> hdr_stats;
            
            if (vm.count("stats")) {
               
               for (unsigned long i = 0; i < numberOfTraces; i++) {
              
                  traceHeader TH;
                  std::streamoff offset = 3600 + i * (240 + num_samples * 4);
                  file.seekg(offset, std::ios::beg);
                  file.read(reinterpret_cast<char*>(&TH), sizeof(traceHeader));
                  if (need_swap) traceHeaderSwapEndian(TH);
                  for (int j = 0; j < bytes_formats.size(); j++) {
                    if (i == 0) {
                        hdr_stats.push_back(Statistics());
                    }
                    else {                      
                        float val = getSpecifiedTraceHeaderValue(TH, bytes_formats[j].first, bytes_formats[j].second);
                        hdr_stats[j].update(val);
                    }
                  }
                  progressBar(i, numberOfTraces);                  
               }  
               clearProgressBar();
               std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << "Header" << " ";
               std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << "Minimum" << " ";
               std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << "Maximum" << " ";
               std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << "Average" << " ";
               std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << "StdDev" << " " << std::endl;
               for (int i = 0; i < hdr_stats.size(); i++) {
                  std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << "B"+std::to_string(bytes_formats[i].first)+\
                  ":F"+std::to_string(bytes_formats[i].second) << " ";
                  std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << std::fixed << std::setprecision(DEFAULT_PRECISION) << hdr_stats[i].min << " ";
                  std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << std::fixed << std::setprecision(DEFAULT_PRECISION) << hdr_stats[i].max << " ";
                  std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << std::fixed << std::setprecision(DEFAULT_PRECISION) << hdr_stats[i].mean << " ";
                  std::cout << std::setw(DEFAULT_COL_WIDTH) << std::right << std::fixed << std::setprecision(DEFAULT_PRECISION) << hdr_stats[i].standardDeviation() << " ";
                  std::cout << std::endl;

               }
            }
        }
       
    }
    else {
        std::cerr << "No file name provided" << std::endl;
        return 1;
    }

    return 0;
}
