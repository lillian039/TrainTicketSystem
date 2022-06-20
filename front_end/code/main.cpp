#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "parser.hpp"

using std::string;
using std::stringstream;

char real_command[100000];
char result[100000];
char real_result[100000];
stringstream real_result_stream;

int main()
{

    int slisten = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(443); //改
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(slisten, (struct sockaddr *)&sin, sizeof(sin)) == -1)
        printf("bind error!");
    if (listen(slisten, 5) == -1)
    {
        printf("listen error!");
        return 0;
    }
    int sClient;
    sockaddr_in remoteAddr;
    socklen_t nAddrlen = sizeof(remoteAddr);
    char revData[99000];

    string commandLine;
    while (true)
    {
        memset(revData,0,sizeof(revData));
        memset(real_command, 0, sizeof(real_command));
        memset(result, 0, sizeof(result));
        memset(real_result, 0, sizeof(real_result));
        sClient = accept(slisten, (struct sockaddr *)&remoteAddr, &nAddrlen);
        if (sClient == -1)
        {
            printf("Accept Error!");
            continue;
        }
        int ret = recv(sClient, revData, 99000, 0);
        stringstream sstr(revData);
        commandLine.clear();
        getline(sstr, commandLine);
        std::cout<<commandLine<<std::endl;
        real_result_stream << parse_command(commandLine);
        strcpy(real_result,real_result_stream.str().c_str());
        send(sClient,real_result,strlen(real_result),0);
        real_result_stream.clear();
        real_result_stream.str("");
    }
}
