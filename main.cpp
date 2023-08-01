#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include "Song.h"

template<typename Base, typename T>
inline bool instanceof(const T*) {
    return std::is_base_of<Base, T>::value;
}

void add_song();

std::list<Song> song_list;
std::list<Song>::iterator song_iterator;

int main() {
    song_list.emplace_back("Song1", "Idiot", 4);
    song_iterator = song_list.begin();

    char current_selection;

    do {
        std::cout << "=============================================" << std::endl;
        std::cout << "Enter a selection (Q to quit):" << std::endl << std::endl;
        std::for_each(song_list.begin(), song_list.end(), [](Song &song) {
            std::cout << song;
        });

        std::cout << std::endl << "Currently Playing:" << std::endl << *song_iterator;
        std::cout << std::endl << std::endl;

        std::cout <<
        "F - Play First Song" << std::endl <<
        "N - Play Next Song" << std::endl <<
        "P - Play Previous Song" << std::endl <<
        "A - Add and Play New Song" << std::endl <<
        "L - List Current Playlist" << std::endl;
        std::cout << "=============================================" << std::endl;

        std::cin >> current_selection;

        switch(toupper(current_selection)) {
            case 'F':
                song_iterator = song_list.begin();
                break;
            case 'N':
                song_iterator++;
                if (song_iterator == song_list.end()) song_iterator = song_list.begin();
                break;
            case 'P':
                if (song_iterator == song_list.begin()) song_iterator = song_list.end();
                song_iterator--;

                break;
            case 'A':
                add_song();
                break;
            case 'L':
            case 'Q':
                break;
            default:
                std::cout << "Invalid Entry: Please try again!" << std::endl;
                break;
        }


    } while(toupper(current_selection) != 'Q');

    std::cout << "Goodbye!" << std::endl;

    return 0;
}

void add_song() {
    std::string name;
    std::string artist;
    int rating;

    std::cout << "Please enter a name for the song: " << std::endl;
    std::cin >> name;
    std::cout << "Please enter the song's artist: " << std::endl;
    std::cin >> artist;

    main:
    do {
        std::string temp;
        std::cout << "Please enter a rating for the song (1-5)" << std::endl;
        std::cin >> temp;

        for(auto c : temp) if(!isdigit(c)) goto main;
        rating = std::stoi(temp);

    } while(rating < 1 || rating > 5);

    song_list.emplace_back(name, artist, rating);
    song_iterator = song_list.begin();
}


