#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>

#include <unistd.h>

// Use: debug-export <input-file> <output-file>
char* args[] = { "debug-exporter", "debug-exporter", "debug-exporter.map", 0 };
int main(int argc, char** argv) {
    argc = 3;
    argv = args;
    if(argc < 3) return 1;
    std::string cmd = "readelf --debug-dump=decodedline ";
    cmd += argv[1];
    FILE* prog_out = popen(cmd.c_str(), "r");
    if(prog_out == 0) {
        std::cerr << "Failed to popen!\n";
        return 2;
    }

    FILE* out_file = fopen(argv[2], "wb");

    char line_buffer[1024];
    while(fgets(line_buffer, sizeof(line_buffer), prog_out) != 0) {
        if(line_buffer[strlen(line_buffer)-2] == ':' || line_buffer[0] == '\n' || !strncmp(line_buffer, "File name", 9) || !strncmp(line_buffer, "Contents", 8)) continue;
        char name_buffer[256];
        uint32_t line_num;
        uint64_t address;

        sscanf(line_buffer, "%s %d 0x%lx", name_buffer, &line_num, &address);
        fwrite(&address, sizeof(uint64_t), 1, out_file);
        fwrite(&line_num, sizeof(uint32_t), 1, out_file);
        fwrite(name_buffer, sizeof(char), strlen(name_buffer)+1, out_file);
    }

    fclose(out_file);
    pclose(prog_out);
    return 0;
}