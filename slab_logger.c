#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 10000
int read_from_shell(const char* cmd, char* buffer, int buffer_length)
{
    FILE* shell_cmd = popen(cmd, "r");
    int total_read = 0;
    int current_read;
    while( current_read = fread(buffer + total_read, sizeof(char), buffer_length - total_read, shell_cmd))
    {
        total_read += current_read;
        if( buffer_length == total_read)
        {
            break;
        }
    }
    pclose(shell_cmd);

    return total_read;

}

char* log_path = "./logs/";
int main(int argc, char* argv[])
{
    
    char buffer[BUFFER_SIZE];
    int buffer_length;
    int i;
    int log_path_len = strlen(log_path);
    for(i = 0; i < log_path_len; i++)
    {
        buffer[i] = log_path[i];
    }
    buffer_length = read_from_shell("date +%s", buffer+log_path_len, BUFFER_SIZE);
    buffer[log_path_len + buffer_length - 1] = '\0';
    FILE* log_file = fopen(buffer, "w");
    double peek = 0;
    double low = 1e20;
    while(1)
    {
        FILE* slab_pipe = popen("cat /proc/slabinfo | awk 'NR > 2 {print $2, $3, $4}'", "r");
        double active_size = 0, total_size = 0;
        while( fgets(buffer, BUFFER_SIZE, slab_pipe))
        {
            int active_objs, num_objs, objsize;
            sscanf(buffer, "%d%d%d", &active_objs, &num_objs, &objsize);
            active_size += active_objs * objsize;
            total_size += num_objs * objsize;
        }
        pclose(slab_pipe);
        active_size /= 1024 * 1024;
        total_size /= 1024 * 1024;
        if( total_size > peek)
        {
            peek = total_size;
        }
        if( total_size < low)
        {
            low = total_size;
        }
     
        system("clear");
        fprintf(stdout, "logging kernel memory usage\n\n\n\n\n\
DO NOT SHUTDOWN THIS COMPUTER!\n\n\n\n\n\n");
        fprintf(stdout, "active slabs size: %.3lfMB\ntotal slabs size:%.3lfMB\n", active_size, total_size);
        fprintf(stdout, "peek: %.3lfMB\nlow: %.3lfMB\n", peek, low);
        fprintf(log_file, "%.6lf %.6lf\n", active_size, total_size); 
        fflush(log_file);
        sleep(10);

    }
    fclose(log_file);
    return  0;
}
