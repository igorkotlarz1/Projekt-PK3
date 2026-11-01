// Movie library: a console app built on a hand-written binary search tree container.
//
// Usage: movie_library [movies_file] [favourites_file]
// Defaults: data/movies.txt and data/favs.txt

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

#include "binary_tree.hpp"
#include "console.hpp"
#include "movie.hpp"

namespace
{
    int current_year()
    {
        std::time_t now = std::time(nullptr);
        return std::localtime(&now)->tm_year + 1900;
    }

    bool validate_data(const std::string& title, const std::string& director,
                       int year, double rating, const std::string& genre)
    {
        return !(title.empty() || director.empty() || genre.empty()
                 || year < 1878 || year > current_year() || rating < 0.0 || rating > 10.0);
    }

    // Reads a number from std::cin until it is valid and within [min, max]
    template<typename Number>
    Number read_number(Number min, Number max, const std::string& error_message)
    {
        std::string input;
        Number value{};
        while (true)
        {
            std::cin >> input;
            std::stringstream ss(input);
            if (ss >> value && ss.eof() && value >= min && value <= max)
                return value;
            std::cout << error_message;
        }
    }
}

class MovieLibrary
{
private:
    const std::string movies_file, fav_file;
    BinaryTree<Movie> movies, favourites;

    MovieLibrary(const std::string& movies_file, const std::string& fav_file)
        : movies_file(movies_file), fav_file(fav_file)
    {
        read_from_file(movies_file, movies, true);
        read_from_file(fav_file, favourites, false);
    }

    // File I/O. Format: title,director,year,rating,genre
    void read_from_file(const std::string& file_name, BinaryTree<Movie>& tree, bool required)
    {
        std::ifstream file(file_name);
        if (!file)
        {
            if (required)
                throw std::runtime_error("Unable to read from file " + file_name + "!");
            return;
        }

        std::string line, title, director, release_year, rating, genre;
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;
            std::stringstream ss(line);
            std::getline(ss, title, ',');
            std::getline(ss, director, ',');
            std::getline(ss, release_year, ',');
            std::getline(ss, rating, ',');
            std::getline(ss, genre, ',');

            try
            {
                int year = std::stoi(release_year);
                double rat = std::stod(rating);
                if (validate_data(title, director, year, rat, genre))
                    tree.add(Movie(title, director, year, rat, genre));
                else
                    std::cout << "Skipping invalid record: " << line << std::endl;
            }
            catch (const std::exception& except)
            {
                std::cout << "Unable to read line: " << line << " (" << except.what() << ")" << std::endl;
            }
        }
    }

    // Saved median-first, so the next load rebuilds a balanced tree.
    // (Saving in sorted order would turn the tree into a linked list on reload.)
    void save_to_file(const std::string& file_name, const BinaryTree<Movie>& tree) const
    {
        std::ofstream file(file_name);
        if (!file)
            throw std::runtime_error("Unable to open file " + file_name + "!");

        tree.balanced_order([&file](const Movie& movie)
        {
            file << movie.getTitle() << ',' << movie.getDirector() << ',' << movie.getYear() << ','
                 << movie.getRating() << ',' << movie.getGenre() << '\n';
        });
    }

    // Library features
    void add_movie()
    {
        std::string title, director, genre;
        std::cout << "Add a movie to the library by entering the data below (type Tab and Enter to leave)" << std::endl;
        std::cout << "Enter movie's title: ";
        std::getline(std::cin, title);
        if (title == "\t" || title.empty())
            return;
        std::cout << "Enter director: ";
        std::getline(std::cin, director);
        std::cout << "Enter movie's genre: ";
        std::getline(std::cin, genre);
        std::cout << "Enter release year: ";
        int year = read_number(1878, current_year(), "Year must be between 1878 and the current year. Try again: ");
        std::cout << "Enter movie's rating: ";
        double rating = read_number(0.0, 10.0, "Rating must be a real number between 0 and 10. Try again: ");

        if (movies.add(Movie(title, director, year, rating, genre)))
            std::cout << "Successfully added " << title << " by " << director << " to the library!" << std::endl;
        else
            std::cout << title << " is already in the library." << std::endl;
    }

    void delete_movie()
    {
        std::string title;
        while (true)
        {
            std::cout << "Find a movie you want to delete by title (type Tab and Enter to leave): ";
            std::getline(std::cin, title);
            if (title == "\t")
                break;
            Movie key(title);
            if (movies.delete_(key))
            {
                favourites.delete_(key);
                std::cout << "Successfully deleted " << key << " from the library!" << std::endl;
                break;
            }
            std::cout << "Failed to find a movie titled " << key << "! Try again." << std::endl;
        }
    }

    void search_title(const std::string& prefix) const
    {
        int count = 0;
        std::cout << "Type in the title: " << prefix << "\n\nMatching movie titles:" << std::endl;
        movies.in_order([&](const Movie& movie)
        {
            if (movie.getTitleUpper().rfind(prefix, 0) == 0)
            {
                std::cout << "- " << movie << std::endl;
                count++;
            }
        });
        if (count == 0)
            std::cout << "No matches found!" << std::endl;
    }

    // Live search: results refresh after every key press, Enter finishes
    void find_movie() const
    {
        std::string input;
        search_title(input);
        while (true)
        {
            int ch = console::read_key();
            if (console::is_enter(ch))
                break;
            if (console::is_backspace(ch))
            {
                if (!input.empty())
                    input.pop_back();
            }
            else if (std::isprint(ch))
                input += static_cast<char>(std::toupper(ch));
            console::clear_screen();
            search_title(input);
        }
    }

    void add_to_favourites()
    {
        std::string title;
        while (true)
        {
            std::cout << "Find a movie you want to save to favourites by title (type Tab and Enter to leave): ";
            std::getline(std::cin, title);
            if (title == "\t")
                return;
            Movie key(title);
            if (movies.find(key))
            {
                movies.copy_to_tree(key, favourites);
                std::cout << "Successfully saved " << key << " to your favourites list!\n" << std::endl;
                show_favourites();
                return;
            }
            std::cout << "Failed to find a movie titled " << key << "! Try again." << std::endl;
        }
    }

    void show_favourites() const
    {
        if (favourites.is_empty())
        {
            std::cout << "There are no movies in your favourites list yet!" << std::endl;
            return;
        }
        std::cout << "Content of your favourites list:\n";
        favourites.in_order([](const Movie& movie)
        {
            std::cout << " - ";
            movie.print_info();
        });
    }

    // by_genre: true groups by genre, false groups by director
    std::map<std::string, BinaryTree<Movie>> group_by(bool by_genre) const
    {
        std::map<std::string, BinaryTree<Movie>> groups;
        movies.in_order([&](const Movie& movie)
        {
            groups[by_genre ? movie.getGenre() : movie.getDirector()].add(movie);
        });
        return groups;
    }

    void print_grouped(bool by_genre) const
    {
        for (const auto& [key, group] : group_by(by_genre))
        {
            std::cout << key << " (" << group.getCounter() << "):" << std::endl;
            group.in_order([](const Movie& movie) { std::cout << " - " << movie.getTitle() << std::endl; });
        }
    }

    void overall_stats() const
    {
        if (movies.is_empty())
        {
            std::cout << "The library is empty." << std::endl;
            return;
        }
        double rating_sum = 0;
        std::map<std::string, int> genre_count, director_count;
        movies.in_order([&](const Movie& movie)
        {
            genre_count[movie.getGenre()]++;
            director_count[movie.getDirector()]++;
            rating_sum += movie.getRating();
        });

        auto by_count = [](const auto& a, const auto& b) { return a.second < b.second; };
        auto top_genre = std::max_element(genre_count.begin(), genre_count.end(), by_count);
        auto top_director = std::max_element(director_count.begin(), director_count.end(), by_count);

        std::cout << "Number of movies in the library: " << movies.getCounter() << '\n'
                  << "Tree height: " << movies.height() << '\n'
                  << "Average movie rating: " << std::fixed << std::setprecision(2)
                  << rating_sum / movies.getCounter() << '\n'
                  << "Most popular genre: " << top_genre->first << " (" << top_genre->second << " movies)\n"
                  << "Most popular director: " << top_director->first << " (" << top_director->second
                  << " movies)" << std::endl;
        std::cout.unsetf(std::ios::fixed);
    }

    void end_session() const
    {
        save_to_file(movies_file, movies);
        save_to_file(fav_file, favourites);
        std::cout << "Thank you for using our services. We hope to see you again soon..." << std::endl;
    }

    // Returns false when the user ends the session
    bool menu_once()
    {
        std::cout << "Choose an action:\n"
                  << "[1] Sort movies by title A - Z\n"
                  << "[2] Sort movies Z - A\n"
                  << "[3] Search movie by title\n"
                  << "[4] Add a movie\n"
                  << "[5] Delete a movie\n"
                  << "[6] Group movies by genres\n"
                  << "[7] Group movies by directors\n"
                  << "[8] Show the overall stats\n"
                  << "[9] Add a movie to favourites\n"
                  << "[10] Show favourites list\n"
                  << "[11] End session\n"
                  << "Choose an action between 1 and 11: ";
        int action = read_number(1, 11, "Unknown action. Try again: ");
        std::cout << std::endl;
        std::cin.ignore();

        switch (action)
        {
        case 1: console::clear_screen(); movies.print(true); break;
        case 2: console::clear_screen(); movies.print(false); break;
        case 3: console::clear_screen(); find_movie(); break;
        case 4: add_movie(); break;
        case 5: delete_movie(); break;
        case 6: print_grouped(true); break;
        case 7: print_grouped(false); break;
        case 8: overall_stats(); break;
        case 9: add_to_favourites(); break;
        case 10: show_favourites(); break;
        default: end_session(); return false;
        }

        console::sleep_ms(1000);
        std::cout << "\nPress [0] to return to the main menu, press [1] to end this session: ";
        if (read_number(0, 1, "Unknown action. Try again: ") == 1)
        {
            end_session();
            return false;
        }
        std::cout << "Returning to the main menu...";
        console::sleep_ms(1000);
        console::clear_screen();
        return true;
    }

public:
    MovieLibrary(const MovieLibrary&) = delete;
    MovieLibrary& operator=(const MovieLibrary&) = delete;

    // Single shared library instance (Meyers singleton)
    static MovieLibrary& getInstance(const std::string& movies_file, const std::string& fav_file)
    {
        static MovieLibrary instance(movies_file, fav_file);
        return instance;
    }

    void run()
    {
        while (menu_once()) {}
    }
};

int main(int argc, char* argv[])
{
    const std::string movies_file = argc > 1 ? argv[1] : "data/movies.txt";
    const std::string fav_file = argc > 2 ? argv[2] : "data/favs.txt";
    try
    {
        MovieLibrary::getInstance(movies_file, fav_file).run();
    }
    catch (const std::exception& except)
    {
        std::cout << except.what() << std::endl;
        return 1;
    }
    return 0;
}
