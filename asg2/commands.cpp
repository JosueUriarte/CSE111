// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },//todo
   {"cd"    , fn_cd    },//todo
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },//todo
   {"lsr"   , fn_lsr   },//todo
   {"make"  , fn_make  },//todo
   {"mkdir" , fn_mkdir },//todo
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },//todo
   {"rmr"   , fn_rmr   },//todo
   {"#"     , fn_hash  },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words){
   //Did not finish
   throw command_error("not implemented");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   //Did not finish
   throw command_error("not implemented");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   int status = 0;

   if(words.size() > 1){

      //This will give us the exit status but in string form
      string exitStat = words.at(1);

      //Loop through the string to make sure there are no
      //non numeric characters
      for(long unsigned int idx = 0; idx < exitStat.size(); idx++){

         if(exitStat.at(idx) < '0'){
            status = 127;
         }

         else if(exitStat.at(idx) > '9'){
            status = 127;
         }

      }

      //if the status isnt 127 then transform the status to
      //a number using stoi
      if(status != 127){
         status = stoi(exitStat);
      }

   }

   exec::status(status);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   //If just looking at the ls of cwd just print direct
   if(words.size() < 2){
      fn_pwd(state, words);
      cout << ":" << endl;
      state.get_cwd()->get_contents()->print_dir();
   }

   else{

      if(words.at(1) == "/"){
         fn_pwd(state, words);
         cout << ":" << endl;
         state.get_root()->get_contents()->print_dir();
      }

   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   //Did not finish
   throw command_error("not implemented");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   //Did not finish
   throw command_error("not implemented");
   DEBUGF ('c', state);
   DEBUGF ('c', words);

}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   //No directory specified, just make the directory
   if(words.size() < 3){
      state.get_contents(state.get_cwd())->mkdir(words.at(1));
   }

}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   string prompt = "";

   for(long unsigned int idx = 1; idx < words.size(); ++idx){
      prompt += words.at(idx) + " ";
   }

   state.set_prompt(prompt);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   //If the root IS the cwd then just print
   if(state.get_root() == state.get_cwd()){

      cout << "/";

      //Make a newline if just printing the pwd
      if(words.at(0) == "pwd"){
         cout << "\n";
      }

      return;
   }

   //Trying to find the parent dir pointer
   inode_ptr parent;
   map<string,inode_ptr> temp = state.get_cwd()->
      get_contents()->get_dirents();

   auto iter = temp.begin();

   while( iter != temp.end() ){

      if(iter->first == ".."){
         parent = iter->second;
         break;
      }

      iter++;
   }

   //Trying to find the current dir pointer
   temp = parent->get_contents()->get_dirents();
   iter = temp.begin();

   while( iter != temp.end() ){

      if(iter->second == state.get_cwd()){
         cout << "/" << iter->first;
         break;
      }

      iter++;
   }


}

void fn_rm (inode_state& state, const wordvec& words){
   //Did not finish
   throw command_error("not implemented");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   //Did not finish
   throw command_error("not implemented");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_hash (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //ignores the users inputs
}
