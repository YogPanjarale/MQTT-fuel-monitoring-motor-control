#include <Arduino.h>
#include <details.h>
struct UserDetails
{
    String username="";
    String password="";
};

UserDetails parseUserDetails(String token)
{
    UserDetails details;
    for (int i = 0; i < token.length(); i++)
    {
        if (token.substring(i, i + 1) == ".")
        {
            details.username = token.substring(0, i);
            details.password = token.substring(i + 1);
            break;
        }
    }
    return details;
}
char* withTopic(String input){
    return (char*) String("devices/"+String(CLIENT_NAME)+input).c_str();
};

