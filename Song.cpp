#include "Song.h"
#include <iomanip>
#include <utility>

void operator<<(std::ostream &os, Song &rhs) {
    os << rhs.name << std::setw(10) << std::right << rhs.artist
    << std::setw(5) << std::right << rhs.rating << std::endl;
}

Song::Song(std::string name, std::string artist, int rating) : name{std::move(name)},
artist{std::move(artist)}, rating{rating} {

}
