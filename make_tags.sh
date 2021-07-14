#!/usr/bin/zsh

rm tags.files || true
find /usr/local/include/gnuradio -type f > tags.files
find /usr/include/c++/ -type f >> tags.files
find /usr/include/qt/ -type f >> tags.files
find /usr/include/GL/ -type f >> tags.files
find /usr/include/boost/ -type f >> tags.files
find include -type f >> tags.files
echo /usr/include/iio.h >> tags.files

ctags --c++-kinds=+p --fields=+iaS --extras=+q -L tags.files

find . -name '*.h' -o -name '*.cc' -o -name '*.c' | grep -vF bindings | grep -vF '/qa_' > cscope.files
cscope -b -q
