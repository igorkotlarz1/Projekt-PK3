#include <iostream>
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

    //Opearotry
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

    //Gettery
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

    void print_info() const
    {
        std::cout << title << " by " << director << ". Rating: " << rating<<"/10"<<std::endl;
    }

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

    //Dodawanie/usuwanie
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

    //Wyszukiwanie
    Node<T>* find_node(Node<T>* current, const T& value) const
    {
        if (!current || current->data == value)
            return current;
        if (value < current->data)
            return find_node(current->left.get(), value);
        else
            return find_node(current->right.get(), value);
    }

    //Przejscie po drzewie
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
    void in_order_func(const std::unique_ptr<Node<T>>& current, const std::function<void(const T&)>& function) const
    {
        if (!current) return;
        in_order_func(current->left,function);
        function(current->data);
        in_order_func(current->right,function);
    }  

    //Pomocnicze
    std::unique_ptr<Node<T>> copy_node(const std::unique_ptr<Node<T>>& current)
    {
        if (!current)
            return nullptr;
        std::unique_ptr<Node<T>> new_node = std::make_unique<Node<T>>(current->data);
        new_node->left = copy_node(current->left);
        new_node->right = copy_node(current->right);

        return new_node;
    }

public:
    BinaryTree() : root(nullptr), counter(0) {}
    //Konstruktor przenoszący
    BinaryTree(BinaryTree&& other) noexcept : root(std::move(other.root)){}

    //Konstruktor kopiujacy
    BinaryTree(const BinaryTree& other) : counter(other.counter)
    {
        root = copy_node(other.root);
    }

    //Dodawanie/usuwanie
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

    //Wyszukiwanie
    Node<T>* find(const T& value) const
    {
        return find_node(root.get(), value);
    }

    //Przejscie po drzewie
    void in_order(const std::function<void(const T&)>& function) const
    {
        in_order_func(root, function);
    }
    void print(bool ascending) const
    {
        in_order_traversal(root, ascending);
    }

    //Dodatkowe
    void copy_to_tree(const T& value, BinaryTree<T>& other_tree) const
    {
        Node<T>* node = find(value);
        if (node)// != nullptr
            other_tree.add(node->data);
    }
    int getCounter() const { return counter; }
    bool is_empty() const { return counter == 0; }

    //Operator przeniesienia
    BinaryTree& operator=(BinaryTree&& other) noexcept
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

class MovieLibrary
{
private:
    const std::string movies_file, fav_file;
    BinaryTree<Movie> movies, favourites;

    MovieLibrary(const std::string& movies_file, const std::string& fav_file): movies_file(movies_file),fav_file(fav_file) 
    { 
        read_from_file(movies_file, movies); 
        read_from_file(fav_file, favourites);
    }
    MovieLibrary(const MovieLibrary&) = delete;
    MovieLibrary operator=(const MovieLibrary&) = delete;  

    //Zapis/odczyt z plikow
    void read_from_file(const std::string& file_name, BinaryTree<Movie>& tree)
    {
        std::ifstream file(file_name);
        std::string line,title,director,release_year,rating,genre;

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
                    tree.add(Movie(title, director, year, rat, genre)); //movie.add
            }
            catch (const std::exception except)
            {
                std::cout << "Unable to properly read data from line: " << line <<"Exception - "<<except.what() << std::endl;
            }           
        }
        file.close();
    }
    void save_to_file(const std::string& file_name, BinaryTree<Movie>& tree)
    {
        if (tree.is_empty())
            return;

        std::ofstream file(file_name);       
        if (!file)
            throw std::runtime_error("Unable to open file " + file_name + " !");
        
        tree.in_order([&file](const Movie& movie) //movies.in_order
            {
                file << movie.getTitle() << ','
                    << movie.getDirector() << ','
                    << movie.getYear() << ','
                    << movie.getRating() << ','
                    << movie.getGenre() << '\n';
            });
        file.close();
    }

    //Funkcjonalnosci bazy
    void add_movie()
    {
        std::string title, director, genre, rating_str;
        double rating;
        std::cout << "Add a movie to the library by entering the vital data below (press Tab to leave)"<<std::endl;
        std::cout << "Enter movie's title : "; std::getline(std::cin, title);
        if (title == "\t")
            return;
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
            std::cout << "Find a movie you want to delete by title (press Tab to leave): "; std::getline(std::cin, title);
            if (title == "\t")
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
    void add_to_favourites()
    {
        std::string title;
        while (true)
        {
            std::cout << "Find a movie you want to save to favourites by title (press Tab to leave): "; std::getline(std::cin, title);
            if (title == "\t")
                return;
            Movie temp_movie(title);
            if (movies.find(temp_movie))
            {
                movies.copy_to_tree(temp_movie, favourites);
                std::cout << "Succesfully saved " << temp_movie << " to your favourites list!\n"<<std::endl;
                show_favourites();
                break;
            }
            else
                std::cout << "Failed to find a movie titled " << temp_movie << "! Try again." << std::endl;
        }
    }
    void show_favourites() const
    {
        if (favourites.is_empty())
            std::cout << "There are no movies in your favourites list yet!" << std::endl;
        else
        { 
        std::cout << "Content of your favourites list:\n";
        favourites.in_order([](const Movie& movie)
            {
                std::cout << " - ";
                movie.print_info();
            });
        }
    }

    std::map<std::string,BinaryTree<Movie>> groupby_genre_or_director(bool genre) 
    {   
        //Legenda:
        //true -> grouping by genre , false -> grouping by director

        std::map<std::string, BinaryTree<Movie>> groups;
        movies.in_order([&](const Movie& movie)
            {
                if(genre)
                    groups[movie.getGenre()].add(movie);
                else
                    groups[movie.getDirector()].add(movie);
            });
        return groups;
    }
    void print_groupedby_genre_or_director(bool genre)
    {
        std::map<std::string, BinaryTree<Movie>> groupes = groupby_genre_or_director(genre);
       
        for (const auto& pair : groupes)
        {
            std::cout<< pair.first <<"("<<pair.second.getCounter() << "): " << std::endl; //genre or director
            pair.second.in_order([](const Movie& movie)     //BinaryTree<Movies>
                {
                    std::cout << " - " << movie.getTitle() << std::endl;
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

    //Wyswietlanie menu, nawigacja
    void end_session()
    {
        std::cout << "Thank you for using our services. We hope to se you again soon..." << std::endl;
        save_to_file(movies_file, movies);
        save_to_file(fav_file, favourites);
    }
    void menu_return()
    {
        std::string action_str;
        int action;
        Sleep(1000);

        std::cout << "\nPress [0] to return to the main menu, press [1] to end this session : ";
        while (true)
        {
            std::cin >> action_str;
            std::stringstream sos(action_str);
            if (sos >> action && (action == 0 || action == 1))
                break;
            else
                std::cout << "Unknown action. Try again: ";
        }
        if (action == 0)
        {
            std::cout << "Returning to the main menu..."; Sleep(1000);
            system("cls");
            menu();
        }
        else
            end_session();       
    }

public:
    ~MovieLibrary(){}
    static MovieLibrary& getInstance(const std::string& movies_file, const std::string& fav_file)
    {
        static MovieLibrary instance(movies_file,fav_file);
        return instance;
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
            <<"[6] Group movies by genres \n"
            <<"[7] Group movies by directors \n"
            <<"[8] Show the overall stats \n"           
            <<"[9] Add a movie to favourites\n"
            <<"[10] Show favourites list\n"
            <<"[11] End session\n"
            <<"Choose an action between 1 and 11: ";

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
            break;
        }
        case 2:
        {
            system("cls");
            print_descending();
            break;
        }
        case 3:
        {
            system("cls");
            find_movie();
            break;
        }
        case 4:
        {
            add_movie();
            break;
        }
        case 5:
        {
            delete_movie();
            break;
        }
        case 6:
        {
            print_groupedby_genre_or_director(true);
            break;
        }
        case 7:
        {
            print_groupedby_genre_or_director(false);
            break;
        }
        case 8:
        {
            overall_stats();
            break;
        }
        case 9:
        {
            add_to_favourites();
            break;
        }
        case 10:
        {
            show_favourites();
            break;
        }
        case 11:
        {
            end_session();
            break;
        }
        default:
            break;
        }
        if(action!=11)
            menu_return();
    }
    BinaryTree<Movie>& getData() { return movies; }  
};

int main()
{
    try
    {
        MovieLibrary& file_reader = MovieLibrary::getInstance("movies.txt","favs.txt");        
        file_reader.menu();       
    }
    catch (const std::exception& except)
    {
        std::cout << except.what() << std::endl;
    }
}


