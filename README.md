# srcstats
A tiny console program to gather and report C++ source files statistics.
This program emerged as an educational example for my C++ lessons.

ISO C++23.

Command line parameters may be directory or file paths.
The program reports statistics over all header files and source files separately and in total.
Errors are reported to stderr.

Files are considered to be provided in ASCII encoding (or compatible 8-bit encoding, not UTF-8 currently).

[Version 1](https://github.com/kuvshinovdr/srcstats/blob/ea00f46adf40f9704c5b940d06ad1a5a5eab27b5/srcstats.cpp): The simplest version. Reads files using std::getline, ignores CRLF vs LF, ignores UTF encodings, ignores possible filesystem exceptions.

[Version 2](https://github.com/kuvshinovdr/srcstats/blob/34bc9b2a73cb1d1a44bf641e637e02ea4b09d74a/srcstats.cpp): Speed-up: reads files into strings as a whole then processes them. CRLF vs LF: removes all CR characters in ahead. New feature: reports time elapsed.

[Version 3](https://github.com/kuvshinovdr/srcstats/blob/2afa6b6c6f0bab581cf0ba8f006b66a4cd4cfa05/srcstats.cpp): Now exceptions are reported and the program should not just silently fall down. Function file_to_string does not print messages to clog anymore reporting errors via File_error exception.
