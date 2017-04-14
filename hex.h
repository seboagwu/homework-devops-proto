#include <string>
#include <string.h>
#include <sstream>
#include <iomanip>

using namespace std;

// ------------------------------------------------------------------
/*!
    Convert a block of data to a hex string
*/
void toHex(
   void *const data,                   //!< Data to convert
   const size_t dataLength,            //!< Length of the data to convert
   std::string &dest                   //!< Destination string
)
{
   unsigned char       *byteData = reinterpret_cast<unsigned char*>(data);
   std::stringstream   hexStringStream;

   hexStringStream << std::hex << std::setfill('0');
   for(size_t index = 0; index < dataLength; ++index)
      hexStringStream << std::setw(2) << static_cast<int>(byteData[index]);
   dest = hexStringStream.str();
}

void toHexStr(
   string& data,                   //!< Data to convert
   const size_t dataLength,            //!< Length of the data to convert
   std::string &dest                   //!< Destination string
)
{
   const unsigned char *byteData = reinterpret_cast<const unsigned char*>(data.c_str());
   std::stringstream   hexStringStream;

   hexStringStream << std::hex << std::setfill('0');
   for(size_t index = 0; index < dataLength; ++index)
   {
      hexStringStream << std::setw(2) << static_cast<unsigned int>(byteData[index]);
   }
   dest = hexStringStream.str();
}

void toHexWSpace(
   string& data,                   //!< Data to convert
   const size_t dataLength,            //!< Length of the data to convert
   std::string &dest                   //!< Destination string
)
{
   const unsigned char *byteData = reinterpret_cast<const unsigned char*>(data.c_str());
   std::stringstream   hexStringStream;

   hexStringStream << std::hex << std::setfill('0');
   for(size_t index = 0; index < dataLength; ++index)
   {
      hexStringStream << std::setw(2) << static_cast<int>(byteData[index]);
      if (index > 1 && index % 4 == 0) hexStringStream << ' ' ;
   }
   dest = hexStringStream.str();
}


// ------------------------------------------------------------------
/*!
    Convert a hex string to a block of data
*/
void fromHex(
   const std::string &in,              //!< Input hex string
   void *const data                    //!< Data store
)
{
   size_t          length      = in.length();
   unsigned char   *byteData   = reinterpret_cast<unsigned char*>(data);

   std::stringstream hexStringStream; hexStringStream >> std::hex;
   for(size_t strIndex = 0, dataIndex = 0; strIndex < length; ++dataIndex)
   {
      // Read out and convert the string two characters at a time
      const char tmpStr[3] = { in[strIndex++], in[strIndex++], 0 };

      // Reset and fill the string stream
      hexStringStream.clear();
      hexStringStream.str(tmpStr);

      // Do the conversion
      int tmpValue = 0;
      hexStringStream >> tmpValue;
      byteData[dataIndex] = static_cast<unsigned char>(tmpValue);
   }
}


using namespace std;

int HexToDec(const char *x, int radix = 16)
{
   unsigned int counter = 0;
   int result = 0;
   const size_t sz_x = strlen(x);

   while (counter < sz_x)
   {
      result = result * radix;

      if ((x[counter] >= '0') && (x[counter] <= '9'))
      {
         result = result + (x[counter] - '0');
      }
      else
      {
         result = result + (toupper(x[counter]) - 'A') + 10;
      }

      ++counter;
   }

   return result;
}

unsigned long long HexToDec_ull(const char *x, int radix = 16)
{
   unsigned int counter= 0;
   unsigned long long result = 0;
   const size_t sz_x = strlen(x);

   while (counter < sz_x)
   {
      result = result * radix;

      if ((x[counter] >= '0') && (x[counter] <= '9'))
         result = result + (x[counter] - '0');
      else
         result = result + (toupper(x[counter]) - 'A') + 10;

      ++counter;
   }
   return result;
}

// convert hex ascii to int
// return true on success, false on failure
bool hexasciitoint(char& ival, char character)
{
   if(character>=48 && character<=57) // [0-9]
   {
      ival = static_cast<char>(character-48);
      return true;
   }
   else if(character>=65 && character<=70) // [A-F]
   {
      ival = static_cast<char>(10+character-65);
      return true;
   }
   else if(character>=97 && character<=102) // [a-f]
   {
      ival = static_cast<char>(10+character-97);
      return true;
   }

   ival = 0;
   return false;
}

// convert array of 8 hex ascii to f32
// The input hexascii is required to be a little-endian representation
// as used in the iridas file format
// "AD10753F" -> 0.9572857022285461f on ALL architectures

bool hexasciitofloat(float& fval, const char * ascii)
{
   // Convert all ASCII numbers to their numerical representations
   char asciinums[8];
   for(unsigned int i=0; i<8; ++i)
   {
      if(!hexasciitoint(asciinums[i], ascii[i]))
      {
         return false;
      }
   }

   unsigned char * fvalbytes = reinterpret_cast<unsigned char *>(&fval);

#if OCIO_LITTLE_ENDIAN
   // Since incoming values are little endian, and we're on little endian
   // preserve the byte order
   fvalbytes[0] = (unsigned char) (asciinums[1] | (asciinums[0] << 4));
   fvalbytes[1] = (unsigned char) (asciinums[3] | (asciinums[2] << 4));
   fvalbytes[2] = (unsigned char) (asciinums[5] | (asciinums[4] << 4));
   fvalbytes[3] = (unsigned char) (asciinums[7] | (asciinums[6] << 4));
#else
   // Since incoming values are little endian, and we're on big endian
   // flip the byte order
   fvalbytes[3] = (unsigned char) (asciinums[1] | (asciinums[0] << 4));
   fvalbytes[2] = (unsigned char) (asciinums[3] | (asciinums[2] << 4));
   fvalbytes[1] = (unsigned char) (asciinums[5] | (asciinums[4] << 4));
   fvalbytes[0] = (unsigned char) (asciinums[7] | (asciinums[6] << 4));
#endif
   return true;
}

void usage()
{
   //toHex(data.c_str(), data.size(), dest);
   //toHexStr(data, data.size() - 1, dest);
}
