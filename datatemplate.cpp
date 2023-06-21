//
// Created by Benjamin Sylvanus on 6/14/23.
//

#include "datatemplate.h"

std::vector<std::vector<std::uint64_t> > readdims(std::string path) {
    std::cout << "Path: " << path << std::endl;
    std::fstream newfile;
    newfile.open(path, std::ios::in); //open a file to perform read operation using file object
    std::vector<std::vector<std::uint64_t> > v;
    if (newfile.is_open()) {   //checking whether the file is open
        std::string tp;
        while (getline(newfile, tp)) {
            std::vector<std::uint64_t> temp;
            size_t pos = 0;
            std::string token;
            std::string delimiter = "\t";
            while ((pos = tp.find(delimiter)) != std::string::npos) {
                token = tp.substr(0, pos);
                temp.push_back(std::stoi(token));
                tp.erase(0, pos + delimiter.length());
            }
            v.push_back(temp);
        }
        newfile.close(); //close the file object.
    }
    return v;
}