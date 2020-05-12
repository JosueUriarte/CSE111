// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
   uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   bigint result;

   //signs are both negative
   if(is_negative && that.is_negative){
      return {uvalue + that.uvalue, true};
   }

   //signs are both positive
   else if(!(is_negative) && !(that.is_negative)){
      return {uvalue + that.uvalue, false};
   }

   //signs are diff so subtract/add according to cases
   else{

      //left is neg right is pos cases
      if(is_negative && !(that.is_negative)){

         //left == right zero has to be positive
         if(uvalue == that.uvalue){
            return {uvalue - that.uvalue, false};
         }

         //uvalue is bigger so sign is - 
         else if(uvalue > that.uvalue){
            return {uvalue - that.uvalue, true};
         }

         //that.uvalue is bigger so sign is +
         else if(uvalue < that.uvalue){
            return {that.uvalue - uvalue, false};
         }
      }

      //left is pos right is neg cases
      if(!(is_negative) && that.is_negative){

         //left == right zero has to be positive
         if(uvalue == that.uvalue){
            return {uvalue - that.uvalue, false};
         }

         //uvalue is bigger so sign is + 
         else if(uvalue > that.uvalue){
            return {uvalue - that.uvalue, false};
         }

         //that.uvalue is bigger so sign is -
         else if(uvalue < that.uvalue){
            return {that.uvalue - uvalue, true};
         }
      }
   }

   return false;

}

bigint bigint::operator- (const bigint& that) const {
   bigint result;

   //signs are both positive so subtract according to cases
   if(!(is_negative) && !(that.is_negative)){

      //left == right zero has to be positive
      if(uvalue == that.uvalue){
         return {uvalue - that.uvalue, false};
      }
      //uvalue is bigger so sign is + 
      else if(uvalue > that.uvalue){
         return {uvalue - that.uvalue, false};
      }

      //that.uvalue is bigger so sign is -
      else if(uvalue < that.uvalue){
         return {that.uvalue - uvalue, true};
      }
   }

   //signs are both negative so add according to cases
   else if(is_negative && that.is_negative){

      //left == right zero has to be positive
      if(uvalue == that.uvalue){
         return {uvalue + that.uvalue, false};
      }
      //uvalue is bigger so sign is - 
      else if(uvalue > that.uvalue){
         return {uvalue + that.uvalue, true};
      }

      //that.uvalue is bigger so sign is +
      else if(uvalue < that.uvalue){
         return {that.uvalue + uvalue, false};
      }
   }

   //signs are different left is pos, right is neg
   //we are going to add
   else if(!(is_negative) && that.is_negative){

      //left == right zero has to be positive
      if(uvalue == that.uvalue){
         return {uvalue + that.uvalue, false};
      }
      //sign is always +
      else if(uvalue > that.uvalue){
         return {uvalue + that.uvalue, false};
      }

      //sign is always +
      else if(uvalue < that.uvalue){
         return {that.uvalue + uvalue, false};
      }
   }

   //signs are different left is neg, right is pos
   //we are going to add
   else if(is_negative && !(that.is_negative)){

      //left == right zero has to be positive
      if(uvalue == that.uvalue){
         return {uvalue + that.uvalue, false};
      }
      //sign is always -
      else if(uvalue > that.uvalue){
         return {uvalue + that.uvalue, true};
      }

      //sign is always -
      else if(uvalue < that.uvalue){
         return {that.uvalue + uvalue, true};
      }
   }

   return false;
}

bigint bigint::operator* (const bigint& ) const {
   bigint result;
   return result;
}

bigint bigint::operator/ (const bigint& ) const {
   bigint result;
   return result;
}

bigint bigint::operator% (const bigint& ) const {
   bigint result;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "")
              << that.uvalue;
}
