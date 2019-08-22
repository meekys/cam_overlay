#pragma once

#include <string>

struct Args
{
    int    argc;
    char **argv;
    std::string defaultInput;
    std::string defaultDisplay;
};

class Config
{
public:
    Config(Args args);

    std::string AppName;
    std::string AppPath;

    std::string dev_name;
    std::string display;
    std::string input;
    std::string output;
    std::string overlay;

    bool stretch;
    bool rotate;
    bool flip_horizontal;
    bool flip_vertical;

    std::string OutputFormat; // Set by IInput

private:
    void Usage(FILE *fp, int argc, char *argv[]);
};