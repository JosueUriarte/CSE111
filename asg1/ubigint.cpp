// $Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "ubigint.h"
#include "debug.h"

//Constructor
ubigint::ubigint (unsigned long that){

   while (that > 0) {
      ubig_value.push_back(that % 10);
      that = that/10;
   }

}

//Constructor
ubigint::ubigint (const string& that){
   DEBUGF ('~', "that = \"" << that << "\"");

   for (auto idx = that.end()-1; idx >= that.begin(); idx--){
      ubig_value.push_back(*idx - '0');
   }

}

ubigint ubigint::operator+ (const ubigint& that) const {
   ubigint result;
   int sum = 0, carry = 0;
   
   long unsigned int idx = 0;

   //Gets the smaller number of the two vector sizes
   long unsigned int minVector = min(ubig_value.size(), 
                                 that.ubig_value.size());

   //Iterate through values until the end of minVector
   for(; idx < minVector; idx++){

      sum = ubig_value.at(idx) + that.ubig_value.at(idx) + carry;

      if(sum >= 10){
         carry = 1;
         sum %= 10;
      }
      else{
         carry = 0;
      }

      result.ubig_value.push_back(sum);
   }

   //if ubig_value is not the minimum then start 
   //solving for the rest

   if(ubig_value.size() > minVector){

      while(idx != ubig_value.size()){

         sum = ubig_value.at(idx) + carry;

         if(sum >= 10){
            carry = 1;
            sum %= 10;
         }
         else{
            carry = 0;
         }

         result.ubig_value.push_back(sum);
         idx++;

      }
   }

   //if that.ubig_value is not the minimum then start 
   //solving for the rest
   if(that.ubig_value.size() > minVector){

      while(idx != that.ubig_value.size()){

         sum = that.ubig_value.at(idx) + carry;

         if(sum >= 10){
            carry = 1;
            sum %= 10;
         }
         else{
            carry = 0;
         }

         result.ubig_value.push_back(sum);
         idx++;
         
      }
   }

   //if final value had a carry push a 1
   if(carry == 1){
      result.ubig_value.push_back(1);
   }

   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");

   ubigint result;
   int sub = 0, carry = 0;
   
   long unsigned int idx = 0;
   //Gets the smaller number of the two vector sizes
   long unsigned int minVector = min(ubig_value.size(), 
                                 that.ubig_value.size());

   //Iterate through values until the end of minVector
   for(; idx < minVector; idx++){

      sub = ubig_value.at(idx) - that.ubig_value.at(idx) - carry;

      if(sub < 0){
         carry = 1;
         sub += 10;
      }
      else{
         carry = 0;
      }

      result.ubig_value.push_back(sub);
   }

   //if ubig_value is not the minimum then start 
   //solving for the rest
   if(ubig_value.size() > minVector){

      while(idx != ubig_value.size()){

         sub = ubig_value.at(idx) - carry;

         if(sub < 0){
            carry = 1;
            sub += 10;
         }
         else{
            carry = 0;
         }

         result.ubig_value.push_back(sub);
         idx++;

      }
   }

   //if that.ubig_value is not the minimum then start 
   //solving for the rest
   if(that.ubig_value.size() > minVector){
      while(idx != that.ubig_value.size()){

         sub = that.ubig_value.at(idx) - carry;

         if(sub < 0){
            carry = 1;
            sub += 10;
         }
         else{
            carry = 0;
         }

         result.ubig_value.push_back(sub);
         idx++;
         
      }
   }

   //remove leading zeros
   while(result.ubig_value.size() > 0 
                  && result.ubig_value.back() == 0){
      result.ubig_value.pop_back();
   }

   return result;
}

ubigint ubigint::operator* (const ubigint& ) const {
   ubigint result(1);
   return result;
}

void ubigint::multiply_by_2() {

}

void ubigint::divide_by_2() {

}

struct quo_rem { ubigint quotient; ubigint remainder; };

quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {

   bool result = false;

   //Will return a TRUE or FALSE according to the logic
   if(ubig_value.size() != that.ubig_value.size()){
      result = (ubig_value.size() < that.ubig_value.size());
   }

   //They are both the same size so time to iterate
   else{

      for(long unsigned int idx = 0; 
                              idx >= ubig_value.size() -1; idx--){

         if(ubig_value.at(idx) != that.ubig_value.at(idx)){

            if(ubig_value.at(idx) > that.ubig_value.at(idx)){
               result = false;
               break;
            }

            if(ubig_value.at(idx) < that.ubig_value.at(idx)){
               result = true;
               break;
            }

         }
      }
   }

   return result;
}

ostream& operator<< (ostream& out, const ubigint& that) {

   //Print the numbers one by one to "out" starting from the back
   //and the to the front to print correctly
   int counter = 0;

   for(int idx = that.ubig_value.size() - 1 ; idx >= 0; idx--){

      //at 69 chars, end with a \ and then newline
      if(counter == 69){
         out << "\\" << "\n";
      }

      //cast that value as a char and add a '0' to get number
      //in char value
      out << static_cast<char>(that.ubig_value.at(idx) + '0');
      counter++;
   }

   return out;
}
