#include <Arduino.h>
#pragma once

class Credentials {
    public:
        Credentials();
        String ssid;
        String pwd;
        String email;
        String upwd;
};