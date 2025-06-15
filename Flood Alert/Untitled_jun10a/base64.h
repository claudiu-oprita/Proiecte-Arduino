#ifndef BASE64_H
#define BASE64_H

#include <Arduino.h>

namespace base64 {
  const char* encode(const char* input) {
    const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    String encoded;
    int val = 0, valb = -6;

    for (int i = 0; input[i]; i++) {
      val = (val << 8) + input[i];
      valb += 8;
      while (valb >= 0) {
        encoded += base64_table[(val >> valb) & 0x3F];
        valb -= 6;
      }
    }

    if (valb > -6) encoded += base64_table[((val << 8) >> (valb + 8)) & 0x3F];
    while (encoded.length() % 4) encoded += '=';

    char* result = new char[encoded.length() + 1];
    strcpy(result, encoded.c_str());
    return result;
  }
}

#endif