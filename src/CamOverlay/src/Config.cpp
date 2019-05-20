#include <getopt.h>

#include <CamOverlay/Config.hpp>

static const char short_options[] = "?d:i:D:o:srhv";

static const struct option
long_options[] = {
    { "help",    no_argument,       NULL, '?' },
    { "device",  required_argument, NULL, 'd' },
    { "input",   required_argument, NULL, 'i' },
    { "display", required_argument, NULL, 'D' },
    { "output",  required_argument, NULL, 'o' },
    { "stretch", no_argument,       NULL, 's' },
    { "rotate",  no_argument,       NULL, 'r' },
    { "fliph",   no_argument,       NULL, 'h' },
    { "flipv",   no_argument,       NULL, 'v' },
    { 0, 0, 0, 0 }
};

Config::Config(Args args)
    : dev_name("/dev/video0"), display("X11"), input("V4L2MMap"), output(""),
      stretch(false), rotate(false), flip_horizontal(false), flip_vertical(false)
{
    auto argc = args.argc;
    auto argv = args.argv;

    for (;;) {
        int idx;
        int c = getopt_long(argc, argv, short_options, long_options, &idx);

        if (-1 == c)
            break;

        switch (c) {
        case 0: /* getopt_long() flag */
            break;

        case '?':
            Usage(stdout, argc, argv);
            exit(EXIT_SUCCESS);

        case 'd':
            dev_name = optarg;
            break;

        case 'i':
            input = optarg;
            break;

        case 'D':
            display = optarg;
            break;

        case 'o':
            output = optarg;
            break;

        case 's':
            stretch = true;
            break;

        case 'r':
            rotate = true;
            break;

        case 'h':
            flip_horizontal = true;
            break;

        case 'v':
            flip_vertical = true;
            break;

        default:
            Usage(stderr, argc, argv);
            exit(EXIT_FAILURE);
        }
    }
}

void Config::Usage(FILE *fp, int /*argc*/, char *argv[])
{
    fprintf(fp,
         "Usage: %s [options]\n\n"
         "Version 1.3\n"
         "Options:\n"
         "-? | --help          Print this message\n"
         "-d | --device name   Video device name [%s]\n"
         "-i | --input         Input module [%s]\n"
         "-o | --output        Output module [auto]\n"
         "-D | --display       Display module [%s]\n"
         "-s | --stretch       Stretch image to screen\n"
         "-r | --rotate        Rotate image 180 degrees\n"
         "-h | --fliph         Flip image horizontally\n"
         "-v | --flipv         Flip image vertically\n"
         "",
         argv[0], dev_name.c_str(), input.c_str(), display.c_str());
}
