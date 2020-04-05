#ifndef CONSTS_H_
#define CONSTS_H_

#include <cstdio>
#include <iostream>

const std::string NSM_VERSION = "2.3.1";

const int LANG_EXPRTK  = -2025;
const int LANG_FPP     = -2026;
const int LANG_LUA     = -2027;
const int LANG_NPP     = -2028;
const int LANG_UNKNOWN = -2029;

const int MODE_BUILD   = -2030;
const int MODE_INTERP  = -2031;
const int MODE_RUN     = -2032;
const int MODE_SHELL   = -2033;
const int MPDE_UNKNOWN = -2034;

const int NSM_BREAK  = -2035;
const int NSM_CONS   = -2036;
const int NSM_CONT   = -2037;
const int NSM_ENDL   = -2038;
const int NSM_ERR    = -2039;
const int NSM_EXIT   = -2040;
const int NSM_KILL   = -2041;
const int NSM_OFILE  = -2042;
const int NSM_QUIT   = NSM_EXIT;
const int NSM_RET    = -2043;
const int NSM_SENTER = -2044;

#endif //CONSTS_H_
