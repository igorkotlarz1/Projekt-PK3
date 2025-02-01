﻿#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <exception>
#include <functional>
#include <map>
#include <conio.h> 
#include <windows.h>

class Movie
{
private:
    std::string title, director, genre;
    int release_year;
    double rating;
public:
    Movie(std::string title, std::string director, int release_year, double rating, std::string genre) 
        : title(title),director(director),release_year(release_year),rating(rating),genre(genre){}
    Movie(std::string title, std::string director, std::string genre, double rating) : title(title), director(director), genre(genre), rating(rating) { release_year = 2000;}
    Movie(std::string title) : title(title) { director = "A"; release_year = 2000; rating = 6.9; genre = "Action"; }

    bool operator <(const Movie& other_movie) const
    {
        return title < other_movie.title;
    }
    bool operator >(const Movie& other_movie) const
    {
        return title > other_movie.title;
    }
    bool operator ==(const Movie& other_movie) const
    {
        return title == other_movie.title;
    }   
    friend std::ostream& operator<<(std::ostream& COUT, const Movie& movie);

    std::string getGenre() const { return genre; }
    std::string getTitle() const { return title; }
    std::string getDirector() const{ return director; }
    double getRating() const { return rating; }
    int getYear() const { return release_year; }

    std::string getTitleUpper() const 
    { 
        std::string temp = title;
        for (size_t i = 0; i < title.size(); i++)        
           temp[i] = std::toupper(title[i]);       
        return temp;
    }

};
class Serie : public Movie
{
private:
    int seasons;
public:
    Serie(std::string title, std::string director, int release_year, double rating, std::string genre, int seasons)
        : Movie(title,director,release_year,rating,genre), seasons(seasons){}
        
};

std::ostream &operator<<(std::ostream& COUT, const Movie & movie)
{
    COUT << movie.title;
    /*, Director(s) : "<< movie.director <<", Year of release : "<< movie.release_year <<
            ", Genre: "<< movie.genre <<", Rating(IMDB): "<< movie.rating<<"/10\n";*/
        return COUT;
}

template<typename T>
class Node
{
public:
    T data;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    Node(T value) : data(std::move(value)), left(nullptr), right(nullptr) {}
};

template<typename T>
class BinaryTree
{
private:
    std::unique_ptr<Node<T>> root;
    int counter;

    void add_node(std::unique_ptr<Node<T>>& current, const T& value)
    {
        if (!current)        
            current = std::make_unique<Node<T>>(std::move(value));               
        else if (value < current->data)
            add_node(current->left, value);
        else if (value > current->data)
            add_node(current->right, value);       
    }

    std::unique_ptr<Node<T>> delete_node(std::unique_ptr<Node<T>> current, const T& value) //Node<T>*
    {
        if(!current)
            return nullptr;
        
        if(value< current->data)
            current-> left = delete_node(std::move(current->left),value);    
        else if(value>current->data)
            current->right = delete_node(std::move(current->right), value);
        else
        {
            if (!current->left && !current->right)
                return nullptr;
            else if (!current->left) //wezel nie ma lewego dziecka
                return std::move(current->right);
            else if (!current->right) //wezel nie ma prawego dziecka
                return std::move(current->left);
            else //wezel ma oby dwoje dzieci
            {
                //Szukamy najmniejszego potomka w prawym poddrzewie metoda pomocnicza
                Node<T>* successor = find_min(current->right.get());
                current->data = successor->data;
                current->right = delete_node(std::move(current->right), successor->data);
            }
        }      
        return current; 
    }
    Node<T>* find_min(Node<T>* current) 
    {
        while (current->left)
            current = current->left.get();
        return current;
    }

    Node<T>* find_node(Node<T>* current, const T& value) const
    {
        if (!current || current->data == value)
            return current;
        if (value < current->data)
            return find_node(current->left.get(), value);
        else
            return find_node(current->right.get(), value);
    }

    void in_order_func(const std::unique_ptr<Node<T>>& current, const std::function<void(const T&)>& function) const
    {
        if (!current) return;
        in_order_func(current->left,function);
        function(current->data);
        in_order_func(current->right,function);
    }
    /*
    void in_order_ascending(const Node<T>* current) const
    {
        if (!current) return;
        in_order_ascending(current->left.get());
        std::cout << current->data << std::endl;
        in_order_ascending(current->right.get());     
    }
    void in_order_descending(const Node<T>* current) const
    {
        if(!current) return;
        in_order_descending(current->right.get());
        std::cout << current->data << std::endl;
        in_order_descending(current->left.get());       
    }*/
    void in_order_traversal(const std::unique_ptr<Node<T>>& current, bool ascending = true) const
    {
        if (!current) 
            return;
        if (ascending)
        {
            in_order_traversal(current->left, ascending);
            std::cout << current->data << std::endl;
            in_order_traversal(current->right, ascending);
        }
        else
        {
            in_order_traversal(current->right, ascending);
            std::cout << current->data << std::endl;           
            in_order_traversal(current->left, ascending);
        }
    }

public:
    BinaryTree() : root(nullptr), counter(0) {}
    //Konstruktor przenoszący
    BinaryTree(BinaryTree&& other) noexcept : root(std::move(other.root)){}

    void add(const T& value)
    {
        add_node(root, std::move(value));
        counter++;
    }
    void delete_(const T& value)
    {
        root = delete_node(std::move(root), value);
        counter--;
    }

    Node<T>* find(const T& value) const
    {
        return find_node(root.get(), value);
    }
    void in_order(const std::function<void(const T&)>& function) const
    {
        in_order_func(root, function);
    }
    /*
    void print_ascending() const
    {
        in_order_ascending(root.get());
    }
    void print_descending() const
    {
        in_order_descending(root.get());
    }*/
    void print(bool ascending) const
    {
        in_order_traversal(root, ascending);
    }
    int getCounter() const { return counter; }
    //Operator przeniesienia
    BinaryTree& operator=(const BinaryTree&& other) noexcept
    {
        if (this != &other)
        {
            root = std::move(other.root);
            other.root = nullptr;
        }
        return *this;
    }
};

bool validate_data(const std::string& title, const std::string& director,
    const int& year, const double& rating, const std::string& genre)
{ 
    return !(title.empty()|| director.empty() || genre.empty() ||year>2024||rating>10.0||rating<0.0);
}

class FileReader
{
private:
    const std::string file_name;
    FileReader(const std::string& file_name):file_name(file_name){ read_from_file(); }
    FileReader(const FileReader&) = delete;
    FileReader operator=(const FileReader&) = delete;

    BinaryTree<Movie> movies, favourites;

    void read_from_file()//const std::string& file_name
    {
        std::ifstream file(file_name);
        std::string line,title,director,release_year,rating,genre,seasons;

        if (!file)
            throw(std::runtime_error("Unable to read from file" + file_name + "!"));
                
        while (std::getline(file, line))
        {
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
                    movies.add(Movie(title, director, year, rat, genre));
            }
            catch (const std::exception except)
            {
                std::cout << "Unable to properly read data from line: " << line <<"Exception - "<<except.what() << std::endl;
            }           
        }
        file.close();
    }
    void save_to_file()//const std::string file_name
    {
        std::ofstream file(file_name);       
        if (!file)
            throw std::runtime_error("Unable to open file " + file_name + " !");
        
        movies.in_order([&file](const Movie& movie)
            {
                file << movie.getTitle() << ','
                    << movie.getDirector() << ','
                    << movie.getYear() << ','
                    << movie.getRating() << ','
                    << movie.getGenre() << '\n';
            });
        file.close();
    }

public:
    ~FileReader(){}
    static FileReader& getInstance(const std::string& file_name)
    {
        static FileReader instance(file_name);
        return instance;
    }
    BinaryTree<Movie>& getData() { return movies; }

    void add_movie()
    {
        std::string title, director, genre, rating_str;
        double rating;
        std::cout << "Enter movie's title : "; std::getline(std::cin, title);
        std::cout << "Enter director : "; std::getline(std::cin, director);
        std::cout << "Enter movie's genre : "; std::getline(std::cin, genre);
        std::cout << "Enter movie's rating : ";
        while (true)
        {
            std::cin >> rating_str;
            std::stringstream sos(rating_str);
            if (sos >> rating && rating >= 0 && rating <= 10)
                break;
            else
                std::cout << "Rating must be a real number between 0 and 10. Try again: ";
        }

       movies.add(Movie(title, director, genre, rating));
       std::cout << "Successfully added a movie " << title << " by " << director << " to the library!" << std::endl;
    }
    void delete_movie()
    {       
        std::string title;
        while (true)
        {
            std::cout << "Find a movie you want to delete by title : "; std::getline(std::cin, title);
            if (title == "back")
                break;
            Movie temp_movie(title);            
            if (movies.find(temp_movie))
            {
                movies.delete_(temp_movie);
                std::cout << "Succesfully deleted " << temp_movie <<" from the library!"<< std::endl;
                break;
            }
            else
                std::cout << "Failed to find a movie titled " << temp_movie << "! Try again." << std::endl;
        }
    }
    void search_title(const std::string& prefix) const
    {
        int count = 0;
        std::cout << "Type in the title: " << prefix << std::endl;
        std::cout << "\nMatching movie titles: " << std::endl;

        movies.in_order([&](const Movie& movie)
            {
                if (movie.getTitleUpper().find(prefix) == 0)
                {
                    std::cout << "- " << movie<< std::endl;
                    count++;
                }
            });
        if (count == 0)
            std::cout << "No matches found!" << std::endl;
    }
    void find_movie() const
    {
        std::string input;
        char ch;
        std::cout << "Find title: ";

        while(true)
        {           
            if (_kbhit())
            {
                ch = _getch();

                if (ch == '\r') break;
                if (ch == '\b' && !input.empty())
                    input.pop_back();
                else
                    input += std::toupper(ch);

                system("cls");
                search_title(input);
            }
        }
    }
    void print_ascending() const
    {
        //movies.print_ascending();
        movies.print(true);
    }
    void print_descending() const
    {
        //movies.print_descending();
        movies.print(false);
    }

    std::map<std::string,BinaryTree<Movie>> groupby_genre() 
    {
        std::map<std::string, BinaryTree<Movie>> genre_groups;
        movies.in_order([&genre_groups](const Movie& movie)
            {
                genre_groups[movie.getGenre()].add(movie);
            });
        return genre_groups;
    }
    void print_groupedby_genre()
    {
        std::map<std::string, BinaryTree<Movie>> genre_groups = groupby_genre();
       
        for (const auto& pair : genre_groups)
        {
            std::cout<< pair.first <<" ("<<pair.second.getCounter() << "): " << std::endl; //genre
            pair.second.in_order([](const Movie& movie) //binary tree
                {
                    std::cout << "  - " << movie.getTitle() << std::endl;
                });
        }
    }
    void overall_stats()
    {
        int movie_count = movies.getCounter();
        double rating = 0;

        std::map<std::string, int> genre_count;
        std::map<std::string, int> director_count;

        movies.in_order([&](const Movie& movie)
            {
                genre_count[movie.getGenre()]++;
                director_count[movie.getDirector()]++;
                rating += movie.getRating();
            });

        //funkcja max_element zwraca iterator do najwiekszego elementu wg funkcji porownujacej
        auto most_pop_genre = std::max_element(genre_count.begin(), genre_count.end(), [](const auto &pair_a, const auto &pair_b)
            {
                return pair_a.second < pair_b.second;
            });
        auto most_pop_director = std::max_element(director_count.begin(), director_count.end(), [](const auto& pair_a, const auto& pair_b)
            {
                return pair_a.second < pair_b.second;
            });

        std::cout << "Number of movies in the library: " << movie_count << '\n'
            << "Average movie rating: " << std::fixed << std::setprecision(2) << rating / movie_count << '\n'
            << "Most popular genre: " << most_pop_genre->first << " (" << most_pop_genre->second << " movies) \n"
            << "Most popular director: " << most_pop_director->first << " (" << most_pop_director->second << " movies)" << std::endl;
    }
    void end_session()
    {
        std::cout << "Thank you for using our services. We hope to se you again soon..." << std::endl;
        save_to_file();
    }
    void menu_return()
    {
        std::string action;
        Sleep(500);
        std::cout << "\nPress [0] to return to the main menu, press [1] to end this session : "; std::cin >> action;
        while (action != "0" && action != "1")
        {
            std::cout << "Unknown action. Try again: "; std::cin >> action;
        }
        if (action[0] == '0')
        {
            std::cout << "Returning to the main menu..."; Sleep(1000);
            system("cls");
            menu();
        }
        else
            end_session();
    }
    void menu()
    {
        std::string action_str;
        int action;

        std::cout << "Choose an action:\n"
            <<"[1] Sort movies by title A - Z\n"
            <<"[2] Sort movies Z - A \n"
            <<"[3] Search movie by title \n"
            <<"[4] Add a movie \n"
            <<"[5] Delete a movie \n"
            //<<"[5] Add to favourites\n"
            //<<"[5] Delete from favourites \n"
            <<"[6] Group movies by genres \n"
            <<"[7] Group movies by directors \n"
            <<"[8] Show the overall stats \n"
            <<"[9] End session\n"
            <<"Choose an action from 1 to 8: ";

        while(true)
        {   std::cin >> action_str;
            std::stringstream sos(action_str);
            if (sos >> action && action >= 1 && action <= 10)
                break;
            else
                std::cout << "Unknown action. Try again: ";    
        }
        std::cout << std::endl;
        std::cin.ignore();

        switch (action)
        {
        case 1:
        {
            system("cls");
            print_ascending();  
            menu_return();
            break;
        }
        case 2:
        {
            system("cls");
            print_descending();
            menu_return();
            break;
        }
        case 3:
        {
            system("cls");
            find_movie();
            menu_return();
            break;
        }
        case 4:
        {
            add_movie();
            menu_return();
            break;
        }
        case 5:
        {
            delete_movie();
            menu_return();
            break;
        }
        case 6:
        {
            print_groupedby_genre();
            menu_return();
            break;
        }
        case 7:
        {
            //print_groupedby_genre();
            menu_return();
            break;
        }
        case 8:
        {
            overall_stats();
            menu_return();
            break;
        }
        case 9:
        {
            end_session();
            break;
        }
        case 10:
        {
            std::cout << "TEST" << std::endl;
        }
        default:
            break;
        }        
    }
};

int main()
{
    try
    {
        FileReader& file_reader = FileReader::getInstance("movies.txt");
        
        file_reader.menu();       
    }
    catch (const std::exception& except)
    {
        std::cout << except.what() << std::endl;
    }
}


