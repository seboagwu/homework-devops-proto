#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <chrono>
//#include <arpa/inet.h>

#include "hex.h"

using namespace std;

constexpr unsigned char BYTE = 2, BYTE4 = 8, BYTE8 = 16;

unsigned int marker = 0;

inline void advance_marker(unsigned char no_of_bytes)
{
   marker += no_of_bytes;
}

bool is_magic_num(const char* c)
{
   unsigned char tmp[] = "4d505337";
   bool ret = false;
   for (int i = 0; i < BYTE4; ++i)
   {
      if (c[i] == tmp[i])
         ret = true;
      else
      {
         ret = false;
         break;
      }
   }
   return ret;
}

unsigned long long no_of_records(string& c)
{
   char tmp[9];
   memset(tmp, 0x0, 9);
   for (int i = 0, j = 10; i < 8; ++i)
      tmp[i] = c[j++];
   return HexToDec_ull(tmp);
}

unsigned long long get_unix_timestamp(const string& c)
{
   string tm(c.begin() + marker, c.begin() + (marker + BYTE4));
   return HexToDec_ull(tm.c_str());
}

string unix_time_to_readable(uint32_t time_date_stamp)
{
   time_t temp = time_date_stamp;
   tm* t = gmtime(&temp);
   stringstream ss; 
   ss << std::put_time(t, "%Y-%m-%d %I:%M:%S %p"); /// gcc 5+
   return ss.str();
}

unsigned long long get_user_id(const string& c)
{
   string tm(c.begin() + marker, c.begin() + (marker + BYTE8));
   return HexToDec_ull(tm.c_str());
}

float get_amount_f(const string& data)
{
   float val = 0;
   string tm(data.begin() + marker, data.begin() + (marker + BYTE8));
   hexasciitofloat(val, tm.c_str());
   return val;
}

int get_version(string& c)
{
   char tmp[3];
   memset(tmp, 0x0, 3);
   tmp[0] = c[8]; tmp[1] = c[9];
   return HexToDec(tmp);
}

int get_record_type(const string& c)
{
   string tm(c.begin() + marker, c.begin() + (marker + BYTE));
   return HexToDec(tm.c_str());
}

const char* get_record_type_human(int t)
{
   switch(t)
   {
      case 0: return "Debit";
      case 1: return "Credit";
      case 2: return "StartAutopay";
      case 3: return "EndAutopay";
      default:
         return "None";
   }

   return "None";
}

bool read_details(string& data)
{
   float user2456938384156277127_amt, debits, credits;
   int autopays_started, autopays_ended;

   /// Header: Magic Number
   bool b = is_magic_num(data.substr(0, 8).c_str());
   if (!b)
      return false;

   advance_marker(BYTE4);

   /// Read version
   get_version(data);
   advance_marker(BYTE);

   /// Read Number OF records
   const unsigned int recs = no_of_records(data);
   advance_marker(BYTE4);

   for (unsigned int i = 1; i < recs; ++i)
   {

      /// Read record type
      int t = get_record_type(data);
      advance_marker(BYTE);

      /// Read Time
      /// get_unix_timestamp(data); /// Avoided calling, since it wasn't asked in Question
      advance_marker(BYTE4);

      /// Read User ID
      unsigned long long user_id = get_user_id(data);
      advance_marker(BYTE8);

      float amt = get_amount_f(data);
      switch (t)
      {
         case 0:
            debits+= amt;
            advance_marker(BYTE8);
            break;

         case 1:
            credits += amt;
            advance_marker(BYTE8);
            break;

         case 2:
            ++autopays_started;
            break;

         case 3:
            ++autopays_ended;
            break;
      }

      if (user_id == 2456938384156277127 && t < 2)
      {
         //cout << "\nUser ID: " << user_id;
         user2456938384156277127_amt += amt;
      }
   }

   cout << "\nTotal Debits: "    << debits;
   cout << "\nTotal Credits : "  << credits;
   cout << "\nAutopay Started: " << autopays_started;
   cout << "\nAutopay Ended  : " << autopays_ended;
   cout << "\nBalance of user ID 2456938384156277127: " << user2456938384156277127_amt;
   cout << "\n\n";

   return true;
}

int main()
{
   ifstream fin("txnlog.dat", ios::binary);
   ostringstream ostrm;

   unsigned char tmp;
   string data, dest;

   if (fin.is_open())
   {
      while (!fin.eof())
      {
         tmp = fin.get();
         ostrm << tmp;
      }
   }

   data = ostrm.str();
   toHexStr(data, data.size() - 1, dest);
   read_details(dest);

   return 0;
}

