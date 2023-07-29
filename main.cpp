#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

struct City {
    std::string name;
    long population;
    double cost;
};

struct Country {
    std::string name;
    std::vector<City> cities;
};

struct Tours {
    std::string title;
    std::vector<Country> countries;
};

int get_length(const std::string& text);
int get_length(int num);
int get_length(double num);

int main() {
    Tours tours
            { "Tour Ticket Prices from Miami",
              {
                      {
                              "Colombia", {
                              { "Bogota", 8778000, 400.98 },
                              { "Cali", 2401000, 424.12 },
                              { "Medellin", 2464000, 350.98 },
                              { "Cartagena", 972000, 345.34 }
                      },
                      },
                      {
                              "Brazil", {
                              { "Rio De Janiero", 13500000, 567.45 },
                              { "Sao Paulo", 11310000, 975.45 },
                              { "Salvador", 18234000, 855.99 }
                      },
                      },
                      {
                              "Chile", {
                              { "Valdivia", 260000, 569.12 },
                              { "Santiago", 7040000, 520.00 }
                      },
                      },
                      { "Argentina", {
                              { "Buenos Aires", 3010000, 723.77 }
                      }
                      },
              }
            };


    static int BUFFER {5};
    static std::string title {"South America Tourism Costs"};


    std::string column_names [] {"Country", "City", "Population", "Price"};
    int column_lengths [] {0, 0, 0, 0};

    for(const auto& country : tours.countries) {
        int country_len {get_length(country.name)};
        if(column_lengths[0] == 0 || column_lengths[0] < country_len) column_lengths[0] = country_len;

        for(const auto& city : country.cities) {
            int city_len {get_length(city.name)};
            if(column_lengths[1] == 0 || column_lengths[1] < city_len) column_lengths[1] = city_len;

            int pop_len {get_length(static_cast<double>(city.population))};
            if(column_lengths[2] == 0 || column_lengths[2] < pop_len) column_lengths[2] = pop_len;

            int price_len {get_length(static_cast<int>(city.cost))};
            if(column_lengths[3] == 0 || column_lengths[3] < price_len) column_lengths[3] = price_len;
        }
    }

    int total_len {0};

    for(int i{}; i < 4; i++) {
        int len {static_cast<int>(column_names[i].length())};
        column_lengths[i] = std::max(column_lengths[i], len);
        total_len += column_lengths[i] + BUFFER;
    }

    int decor_size {(total_len - (get_length(title) + 1)) / 2};
    std::cout << std::setfill('=') << std::setw(decor_size) << "[" << title << "]"
              << std::setw(decor_size + (get_length(title) % 2 == 0 ? 0 : 1)) << "" << std::endl << std::setfill(' ');

    for(int i {}; i < 4; i++)  std::cout << std::setw(column_lengths[i] + BUFFER) << std::left << column_names[i];

    std::cout << std::endl << std::setfill('=') << std::setw(total_len) << "" << std::setfill('.') << std::endl;

    for(int i {}; i < tours.countries.size(); i++) {
        Country &country = tours.countries.at(i);
        int country_len {static_cast<int>(country.name.length())};

        for(int j {}; j < country.cities.size(); j++) {
            City &city = country.cities.at(j);

            int row_len {(column_lengths[0] + BUFFER) - (j == 0 ? country_len : 0)};
            if(j == 0) std::cout << country.name;

            int city_len {get_length(city.name)};
            int pop_len {get_length(static_cast<int>(city.population))};
            int price_len {get_length(city.cost)};

            std::cout << std::right << std::setw((row_len) + city_len) << city.name;
            row_len = (column_lengths[1] + BUFFER) - city_len;

            std::cout << std::right << std::setw(row_len + pop_len) << city.population;
            row_len = (column_lengths[2] + BUFFER) - pop_len;

            std::cout << std::right << std::setw(row_len + price_len) << city.cost << std::endl;
        }
    }

    std::cout << std::setfill('=') << std::setw(total_len) << "" << std::setfill('.') << std::endl;

    return 0;
}

int get_length(const std::string &text) {
    return static_cast<int>(text.length());
}

int get_length(int num) {
    return static_cast<int>(std::to_string(num).length());
}

int get_length(double num) {
    return static_cast<int>(std::to_string(num).length());
}