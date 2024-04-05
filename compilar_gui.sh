g++ -w -std=c++2a -Wall -Werror -Wconversion `pkg-config --cflags --libs gtk+-3.0`-export-dynamic -rdynamic -lm -c -o obj/user_view.o lib/user_view.cpp
g++ -w -std=c++2a -Wall -Werror -Wconversion `pkg-config --cflags --libs gtk+-3.0`-export-dynamic -rdynamic -lm -c -o obj/main_view.o lib/main_view.cpp
g++ -o programa -w -std=c++2a -Wall -Werror -Wconversion src/reloj.cpp obj/user_view.o obj/main_view.o `pkg-config --cflags --libs gtk+-3.0`
