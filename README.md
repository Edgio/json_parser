json_parser
===========

Lightweight, header only DOM json_parser. Fairly fast and fairly light. Built to handle most JSON. Any encoded/escaped test fields in the JSON will still be encoded/escaped after they are parsed. It is up to the user to decide which fields need unencoding. 

The whitebox tests are the only files that need built. The rest of the files are header only implementations so just include them and use them.

Build the whitebox tests:
    cd build;
    cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../tests
    make

Build the whitebox tests for profiling with gcov:
    cd build;
    cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../tests
    make

Note: The whitebox_aton test takes a while because it is doing timings. There is a noticable difference in the time to run it between Release and Debug.
