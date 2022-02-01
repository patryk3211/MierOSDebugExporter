#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <list>

// Use: debug-export <input-file> <output-file>
int main(int argc, char** argv) {
    if(argc < 3) return 1;
    std::string cmd = "readelf --debug-dump=decodedline ";
    cmd += argv[1];
    FILE* prog_out = popen(cmd.c_str(), "r");
    if(prog_out == 0) {
        std::cerr << "Failed to popen!\n";
        return 2;
    }

    FILE* out_file = fopen(argv[2], "wb");

    struct record {
        uint64_t address;
        uint32_t line_num;
        std::string name;

        bool operator<(const record& other) {
            return address < other.address;
        }
    };
    std::list<record> records;

    char line_buffer[1024];
    while(fgets(line_buffer, sizeof(line_buffer), prog_out) != 0) {
        if(line_buffer[strlen(line_buffer)-2] == ':' || line_buffer[0] == '\n' || !strncmp(line_buffer, "File name", 9) || !strncmp(line_buffer, "Contents", 8)) continue;
        char name_buffer[256];
        uint32_t line_num;
        uint64_t address;

        sscanf(line_buffer, "%s %d 0x%lx", name_buffer, &line_num, &address);
        records.push_back({
            address,
            line_num,
            name_buffer
        });
    }

    records.sort();
    for(auto& r : records) {
        fwrite(&r.address, sizeof(uint64_t), 1, out_file);
        fwrite(&r.line_num, sizeof(uint32_t), 1, out_file);
        fwrite(r.name.c_str(), sizeof(char), r.name.length()+1, out_file);
    }

    fclose(out_file);
    pclose(prog_out);
    return 0;
}