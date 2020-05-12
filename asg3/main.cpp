// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}
 
//Similar to matchlines.cpp
void getInput(istream& infile, string prompt, str_str_map& test){

   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   regex empty_regex {R"(^\s*?$)"};

   int line_count = 1;

   for(;;){

      string line;
      smatch result;

      getline (infile, line);
      if (infile.eof()) break;

      //Nothing inputted. Just continue.
      if(regex_search(line, result, empty_regex)) {
         cout << prompt << ": " << line_count << ": " << endl;
         line_count++;
         continue;
       }
      
      // For comments. Ignore rest.
      if (regex_search(line, result, comment_regex)){
         cout << prompt << ": " << line_count << ": "<< line << endl;
         line_count++;
         continue;
      }

      //Handles most functions
      if(regex_search(line, result, key_value_regex)){

         // = (All keys are printed lexographically)
         if((result[1] == "") && (result[2] == "")){

            auto iter = test.begin();

            cout << prompt << ": " << line_count << ": "<< line << endl;

            for(;iter != test.end();++iter){
               cout << iter->first << " = " << iter->second << endl;
            }

         }

         // key= (Key and value pair is deleted from map)
         else if((result[1] != "") && (result[2] == "")){

            auto iter = test.find(result[1]);
            cout << prompt << ": " << line_count << ": "<< line << endl;
            test.erase(iter);
         }

         // =value (All keys with that value are printed)
         else if((result[1] == "") && (result[2] != "")){

            auto iter = test.begin();

            cout << prompt << ": " << line_count << ": "<< line << endl;

            //Use similar iterator for '='
            for(;iter != test.end();++iter){

               if(iter->second == result[2]){
                  cout << iter->first << " = " << iter->second << endl;
               }
            }

         }

         //key=value 
         //(Key is inserted & printed. if not, value is replaced)
         else if((result[1] != "") && (result[2] != "")){

            str_str_pair pair(result[1], result[2]);

            //Insert the pair and print insertion
            test.insert(pair);
            cout << prompt << ": " << line_count << ": "<< line << endl;
            cout << result[1] << " = " << result[2] << endl;

         }

      }

      // key (Prints the key and value.)
      // ("key: not found" if nonexistant.)
      else if (regex_search (line, result, trimmed_regex)) {

         cout << prompt << ": " << line_count << ": "<< line << endl;

         //Try and find the key in the map
         auto findKey = test.find(result[1]);

         //did not find the key
         if(findKey == test.end()){
            cout << line << ": key not found" << endl;
         }

         //found the key. Print it out
         else{
            cout << line << " = " << findKey->second << endl;
         }
      }

      line_count++;

   }

}


int main (int argc, char** argv) {

   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map test;
   str_str_map inputs;

   str_str_pair default_input ("-", " ");

   int status = 0;
   int fileIdx = 1;

   //Insert words into map
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      test.insert(pair);
   }

   //If the map is empty, insert the default input
   if(test.empty()){
      test.insert(default_input);
   }

   //Run the program
   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {

      //Run the program through the CMD line
      if (itor->first == "-"){
         cin.clear();
         getInput(cin, "-", inputs);
      }

      else{

         ifstream infile(itor->first);

         if (infile.fail()) {
            cerr << sys_info::execname() << ": " << itor->first << ": " 
            "No such file or directory" << endl;
            status = 1;
         }
         else {
            str_str_map fileMap;
            getInput(infile, argv[fileIdx], fileMap);
            infile.close();
         }
      }

      fileIdx++;
   }

   return status;
}
