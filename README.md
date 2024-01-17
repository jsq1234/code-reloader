# Code Reloader

A hot-code loader which reloads in response to changes in test.cpp and runs the program. It moniters the file using inotify
and links the libmytest.so shared library in runtime using linux-specific API.

To use the application, run ```Make```
