#ifndef ELIJAH_HPP
#define ELIJAH_HPP

#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

struct StationData {
    short min_temp;
    short max_temp;
    int total_temp;
    int count;

    StationData() : min_temp(std::numeric_limits<short>::max()), 
                    max_temp(std::numeric_limits<short>::lowest()), 
                    total_temp(0), 
                    count(0) {}
};

inline short str_temp_to_int(std::string_view value) {
    bool negative = false;
    std::size_t pos = 0;

    if (value[pos] == '-') {
        negative = true;
        ++pos;
    } else if (value[pos] == '+') {
        ++pos;
    }

    short whole = 0;
    while (value[pos] != '.') {
        whole = whole * 10 + (value[pos] - '0');
        pos++;
    }

    short decimal = 0;
    ++pos;
    decimal = value[pos] - '0';

    short result = whole * 10 + decimal;
    return negative ? -result : result;
}

inline void chunk_data(std::unordered_map<std::string, StationData>& station_data) {
    int BUFFER_SIZE = 1080;
    FILE *fp;
    char* buffer = new char[BUFFER_SIZE+1];
    size_t result;
    fp = fopen("measurements.txt", "rb");
    char* remaining_buffer = new char[30];
    remaining_buffer[0] = '\0';
    char* new_buffer = new char[1111];
    char* station = new char[30];
    char* temp = new char[30];

    char* previous = new_buffer;
    char* next = strchr(new_buffer, ';');
    char* last = strrchr(new_buffer, '\n');

    while((result = fread(buffer, 1, BUFFER_SIZE, fp)) > 0){
        
        if(remaining_buffer[0] == '\0'){
            strcpy(new_buffer, strcat(remaining_buffer, buffer));
        }
        else{
            strcpy(new_buffer, buffer);
        }

        if(new_buffer[result-1] != '\n'){
            new_buffer[result] = '\n';
        }

        previous = new_buffer;
        next = strchr(new_buffer, ';');
        last = strrchr(new_buffer, '\n');

        while(strchr(previous, '\n') != last && next != nullptr){
            station = strndup(previous, (next - previous));
            
            previous = next + 1;
            next = strchr(previous, '\n');
            temp = strndup(previous, (next - previous));

            short temperature = str_temp_to_int(temp);

            if(station_data.find(station) == station_data.end()){
                station_data.insert({station, StationData()});
            }

            if(station_data[station].max_temp < temperature){
                station_data[station].max_temp = temperature;
            }
            if(station_data[station].min_temp > temperature){
                station_data[station].min_temp = temperature;
            }
            station_data[station].total_temp += temperature;
            station_data[station].count++;

            previous = next + 1;
            next = strchr(previous, ';');

        }
        remaining_buffer = strndup(previous, (last - previous));
        // Process the buffer data here
    }
    free(buffer);
    free(remaining_buffer);
    free(new_buffer);
    free(station);
    free(temp);
    fclose(fp);
}

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string& filename) {

    std::unordered_map<std::string, StationData> station_data{};
    chunk_data(station_data);

    /*std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file \n";
        return;
    }
    
    //std::unordered_map<std::string, std::vector<float>, std::less<std::string>> station_data{};
    station_data.reserve(413);

    std::string line;
    
    while (std::getline(inputFile, line)) {
        size_t delimiter_pos = line.find(';');

        if (delimiter_pos == std::string::npos) {
            break;
        }

        std::string station_name = line.substr(0, delimiter_pos);
        short temperature = str_temp_to_int(line.substr(delimiter_pos + 1));

        if(station_data.find(station_name) == station_data.end()){
            station_data.insert({station_name, StationData()});
        }

        if(station_data[station_name].max_temp < temperature){
            station_data[station_name].max_temp = temperature;
        }
        if(station_data[station_name].min_temp > temperature){
            station_data[station_name].min_temp = temperature;
        }
        station_data[station_name].total_temp += temperature;
        station_data[station_name].count++;
    }

    inputFile.close();
    */
    std::ofstream outputFile("output.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file \n";
        return;
    }

    for (const auto& [station_name, data] : station_data) {
        float average = data.total_temp / data.count;
        outputFile << std::fixed << std::setprecision(1) << station_name << ";" << data.min_temp << ";" << average << ";" << data.max_temp << "\n";
    }
}

#endif //ELIJAH_HPP