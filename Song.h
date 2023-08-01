#ifndef LEARNINGCPP_SONG_H
#define LEARNINGCPP_SONG_H

#include <iostream>


class Song {
    friend void operator<<(std::ostream &os, Song &rhs);

    std::string name;
    std::string artist;
    int rating;

public:
    Song(std::string name, std::string artist, int rating);
};


#endif
