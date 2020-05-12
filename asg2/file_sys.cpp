// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");

   //Points at directory
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   
   //Set pointers to self and its parent
   get_contents(root)->parent_helper(root, root);
   cwd = root;

}

//Destruct the inode_state
inode_state::~inode_state() {
   root->destruct();
   root = nullptr;
}


const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

const inode_ptr inode_state::get_cwd() {
   return cwd;
}

const inode_ptr inode_state::get_root() {
   return root;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}


base_file_ptr inode_state::get_contents(inode_ptr point) {
   return point->get_contents();
}
 

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

base_file_ptr inode::get_contents() {
   return contents;
}

void inode::destruct() {
   if(contents->get_file_type() == "dir"){
      contents->destruct();
   }
   contents = nullptr;
}

file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}

string base_file::get_file_type(){
   throw file_error ("is a " + error_file_type());
}

void base_file::print_dir (){
   throw file_error ("is a " + error_file_type());
}

void base_file::parent_helper (inode_ptr, inode_ptr){
   throw file_error ("is a " + error_file_type());
}

map<string, inode_ptr> base_file::get_dirents() {
   throw file_error ("is a " + error_file_type());
}

void base_file::destruct(){
   throw file_error ("is a " + error_file_type());
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}

void plain_file::set_contents(const map<string, inode_ptr>&){
   throw file_error("is a plain file");
}

size_t directory::size() const {
   size_t size = dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);

   auto iter = dirents.find(dirname);

   if(iter != dirents.end()){
      throw file_error (dirname + " already exists");
   }

   inode_ptr new_dir = make_shared<inode>(file_type::DIRECTORY_TYPE);
   base_file_ptr new_contents = new_dir->get_contents();

   new_contents->parent_helper(new_dir, dirents.at("."));
   dirents.insert(make_pair(dirname, new_dir));

   return new_dir;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

void directory::print_dir(){

   //Begin at the begining of dirents and loop until the end
   for(auto iter = dirents.begin(); iter != dirents.end(); iter++){

      //print white space and the node number
      cout << "      " << iter->second->get_inode_nr();
      //print white space and the content size of node
      cout << "      " << iter->second->get_contents()->size();
      //print white space and the name of the node. Newline
      cout << " " << iter->first << endl;

   }

}

void directory::parent_helper(inode_ptr self, inode_ptr parent) {
   dirents.insert(make_pair(".", self));
   dirents.insert(make_pair("..", parent));
}

map<string, inode_ptr> directory::get_dirents() {
   return dirents;
}

void directory::set_contents(const map<string, inode_ptr>& new_map){
   dirents = new_map;
}

void directory::destruct(){

   auto iter = dirents.begin();

   for(; iter != dirents.end(); ++iter){

      if( (iter->first!=".") && (iter->first!="..") ){

         iter->second->destruct();
      }

      iter->second = nullptr;
   }
}

