# srcstats
A small console program to gather and report C++ source files statistics.
This program emerged as an educational example for my C++ lessons.

ISO C++23.

Command line parameters may be directory or file paths or --help for minimal help/about message.
The program reports statistics over all header files and source files separately and in total as is and after decommenting and removing empty lines and whitespace line endings.
Errors are reported to stderr.

Files are considered to be provided in ASCII encoding (or compatible 8-bit encoding, not UTF-8 currently).


## Change log

[Version 0.7]: Added C# support (untested!).

[Version 0.6](https://github.com/kuvshinovdr/srcstats/tree/551f94c0273eecc9c4ef6dfb794525cfb7beb1fa): Better support for adding input programming languages.

[Version 0.5](https://github.com/kuvshinovdr/srcstats/tree/b56b627d5b3df9d1097db49c3e72585ea134f75f): Refactored to ease adding of several input programming languages support.

[Version 0.4](https://github.com/kuvshinovdr/srcstats/tree/76c8628930c1ccde5a1189f622b7bcc020acea6e): Complete refactoring. Separate file/line statistics and raw/decommented source files statistics. The source code is now split into several header and source files, its total volume is threefold of the version 0.3.

[Version 0.3](https://github.com/kuvshinovdr/srcstats/blob/5d85e23bb4811f4f7cb871aae3ed8f0f31ac3c91/srcstats.cpp): Now exceptions are reported and the program should not just silently fall down. Function file_to_string does not print messages to clog anymore reporting errors via File_error exception.

[Version 0.2](https://github.com/kuvshinovdr/srcstats/blob/34bc9b2a73cb1d1a44bf641e637e02ea4b09d74a/srcstats.cpp): Speed-up: reads files into strings as a whole then processes them. CRLF vs LF: removes all CR characters in ahead. New feature: reports time elapsed.

[Version 0.1](https://github.com/kuvshinovdr/srcstats/blob/ea00f46adf40f9704c5b940d06ad1a5a5eab27b5/srcstats.cpp): The simplest version. Reads files using std::getline, ignores CRLF vs LF, ignores UTF encodings, ignores possible filesystem exceptions.


