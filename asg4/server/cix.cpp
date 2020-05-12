// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog(cout);
struct cix_exit : public exception{};

unordered_map<string, cix_command> command_map{
    {"exit", cix_command::EXIT},
    {"help", cix_command::HELP},
    {"ls"  , cix_command::LS  },
    {"get" , cix_command::GET },
    {"put" , cix_command::PUT },
    {"rm"  , cix_command::RM  },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

//A list of helpful commands
void cix_help(){
   cout << help;
}

//Shows the files that are in the server
void cix_ls(client_socket &server){
   cix_header header;
   header.command = cix_command::LS;
   outlog << "sending header " << header << endl;
   send_packet(server, &header, sizeof header);
   recv_packet(server, &header, sizeof header); 
   outlog << "received header " << header << endl;
   if (header.command != cix_command::LSOUT){
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }
   else{
      auto buffer = make_unique<char[]>(header.nbytes + 1);
      recv_packet(server, buffer.get(), header.nbytes);
      outlog << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}

void cix_get(client_socket &server, string filename){
   cix_header header;
   header.command = cix_command::GET;
   snprintf(header.filename, filename.length() + 1, filename.c_str());

   outlog << "sending header " << header << endl;
   send_packet(server, &header, sizeof header);
   recv_packet(server, &header, sizeof header);
   outlog << "received header " << header << endl;

   if (header.command != cix_command::FILEOUT){
      outlog << "ERROR: couldnt find file: " << header.filename << endl;
      outlog << "server returned " << header << endl;
   }

   auto buffer = make_unique<char[]>(header.nbytes + 1);
   recv_packet(server, buffer.get(), header.nbytes);
   outlog << "received " << header.nbytes << " bytes" << endl;
   buffer[header.nbytes] = '\0';
   ofstream outfile(filename);
   outfile << buffer.get();

   outfile.close();
   outlog << filename << " saved to client successfully!" << endl; 
}

void cix_put(client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::PUT;
   snprintf(header.filename, filename.length() + 1, filename.c_str());
   std::ifstream file(header.filename, std::ifstream::binary);
   
   //Getting the length of the file
   file.seekg (0, file.end);
   size_t length = file.tellg();
   file.seekg (0, file.beg);
   char buffer[length];

   //Exit the cix_put if file not found
   if (file.fail()){
      outlog << "ERROR: couldnt find file: " << header.filename << endl;
      return;
   }

   file.read(buffer, length);

   header.nbytes = length;

   outlog << "sending header " << header << endl;
   send_packet(server, &header, sizeof header);
   send_packet(server, buffer, length);
   outlog << "Sent " << length << " bytes" << endl;
   recv_packet(server, &header, sizeof header);
   outlog << "received header " << header << endl;

   //Confirmation or Error messages
   if (header.command == cix_command::ACK){
      outlog << "Successfully sent " 
                     << filename << " to server!"<< endl;
   }
   else{
      outlog << "sent PUT, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
      outlog << "put: "
       << filename << ": "<< strerror(header.nbytes) << endl;
   }
}

void cix_rm(client_socket &server, string &filename){
   cix_header header;
   header.command = cix_command::RM;
   snprintf(header.filename, filename.length() + 1, filename.c_str());

   outlog << "sending header " << header << endl;
   send_packet(server, &header, sizeof header);
   recv_packet(server, &header, sizeof header);
   outlog << "received header " << header << endl;

   //Confirmation or Error messages
   if (header.command == cix_command::ACK){
      outlog << "Successfully removed " 
                     << filename << " from the server!"<< endl;
   }
   else{
      outlog << "sent RM, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
      outlog << "rm: "
       << filename << ": "<< strerror(header.nbytes) << endl;
   }
}

void usage(){
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main(int argc, char **argv){
   outlog.execname(basename(argv[0]));
   outlog << "starting" << endl;
   vector<string> args(&argv[1], &argv[argc]);
   if (args.size() > 2)usage();

   string host = get_cix_server_host(args, 0);
   in_port_t port = get_cix_server_port(args, 1);
   outlog << to_string(hostinfo()) << endl;

   try{
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server(host, port);
      outlog << "connected to " << to_string(server) << endl;
      for (;;){
         string line;
         getline(cin, line);
         string command_name{}, filename{};
         size_t split_pos = line.find(" ");

         if (split_pos == string::npos){
            command_name = line;
         }

         else{
            command_name = line.substr(0, split_pos);
            filename = line.substr(split_pos + 1);
         }
         
         if (cin.eof())
            throw cix_exit();
         outlog << "command " << line << endl;
         const auto &itor = command_map.find(command_name);
         cix_command cmd = itor == command_map.end()
                               ? cix_command::ERROR: itor->second;
         switch (cmd){
         case cix_command::EXIT:
            throw cix_exit();
            break;
         case cix_command::HELP:
            cix_help();
            break;
         case cix_command::LS:
            cix_ls(server);
            break;
         case cix_command::PUT:
            cix_put(server, filename);
            break;
         case cix_command::GET:
            cix_get(server, filename);
            break;
         case cix_command::RM:
            cix_rm(server, filename);
            break;
         default:
            outlog << line << ": invalid command" << endl;
            break;
         }
      }
   }
   catch (socket_error &error){
      outlog << error.what() << endl;
   }
   catch (cix_exit &error){
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}
