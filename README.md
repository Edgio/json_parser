json_parser
===========

Lightweight, header only DOM json_parser. Fairly fast and fairly light. Built to handle most JSON. Any encoded/escaped test fields in the JSON will still be encoded/escaped after they are parsed. It is up to the user to decide which fields need unencoding. 


Build the whitebox tests:
    cd build;
    cmake -G "Unix Makefiles" ../tests
    make
