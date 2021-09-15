#include <Arduino.h>
#ifndef CONVERTERS_Y
#define CONVERTERS_Y
struct UserDetails
{
    String username;
    String password;
};
UserDetails parseUserDetails(String token);
char* withTopic(String input);
#endif