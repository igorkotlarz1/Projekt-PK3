#pragma once

#include <cctype>
#include <iostream>
#include <string>

// A single movie record. Movies are ordered and compared by title only,
// which is what the binary search tree uses as its key.
class Movie
{
private:
    std::string title, director, genre;
    int release_year;
    double rating;

public:
    Movie(std::string title, std::string director, int release_year, double rating, std::string genre)
        : title(std::move(title)), director(std::move(director)), genre(std::move(genre)),
          release_year(release_year), rating(rating) {}

    // Lookup key: only the title matters for comparisons
    explicit Movie(std::string title)
        : title(std::move(title)), director("-"), genre("-"), release_year(0), rating(0.0) {}

    // Comparison operators (by title)
    bool operator<(const Movie& other) const { return title < other.title; }
    bool operator>(const Movie& other) const { return title > other.title; }
    bool operator==(const Movie& other) const { return title == other.title; }

    friend std::ostream& operator<<(std::ostream& out, const Movie& movie)
    {
        return out << movie.title;
    }

    // Getters
    const std::string& getTitle() const { return title; }
    const std::string& getDirector() const { return director; }
    const std::string& getGenre() const { return genre; }
    double getRating() const { return rating; }
    int getYear() const { return release_year; }

    std::string getTitleUpper() const
    {
        std::string upper = title;
        for (char& c : upper)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        return upper;
    }

    void print_info() const
    {
        std::cout << title << " (" << release_year << ") by " << director
                  << ". Rating: " << rating << "/10" << std::endl;
    }
};
