#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <exception>
#include <functional>
#include <conio.h> 

class Movie
{
private:
    std::string title, director, genre;
    int release_year;
    double rating;
public:
    Movie(std::string title, std::string director, int release_year, double rating, std::string genre) 
        : title(title),director(director),release_year(release_year),rating(rating),genre(genre){}
    Movie(std::string title, std::string director) : title(title), director(director) { release_year = 6969; rating = 6.9; genre = "Action"; }
    Movie(std::string title) : title(title) { director = "Ligma"; release_year = 2000; rating = 6.9; genre = "Action"; }

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

    void add_node(std::unique_ptr<Node<T>>& current, const T& value)
    {
        if (!current)
            current = std::make_unique<Node<T>>(std::move(value));
        else if (value < current->data)
            add_node(current->left, value);
        else if (value > current->data)
            add_node(current->right, value);
    }
    std::unique_ptr<Node<T>> delete_node(std::unique_ptr<Node<T>>& current, T& value) //Node<T>*
    {
        if(!current)
            return current;
        
        if(value< current->data)
            current-> left = std::move(delete_node(current->left,value));    
        else if(value>current->data)
            current->right = std::move(delete_node(current->right,value));
        else
        {
            if (!current->left && !current->right)
                return nullptr;
            else if (!current->left) //wezel nie ma lewego dziecka
                return std::move(current->right);//return current.release();
            else if (!current->right) //wezel nie ma prawego dziecka
                return std::move(current->left); //return current.release();
            else //wezel ma oby dwoje dzieci
            {
                //Szukamy najmniejszego potomka w prawym poddrzewie metoda pomocnicza
                Node<T>* successor = find_min(current->right.get());
                current->data = successor->data;

                current->right = std::move(delete_node(current->right, successor->data));
            }
        }
        return std::move(current); // current.get();
    }
    Node<T>* find_min(Node<T>* current) const
    {
        if (!current || !current->left)
            return current.get();
        return find_min(current->left);
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
    }

public:
    BinaryTree() : root(nullptr) {}

    void add(const T& value)
    {
        add_node(root, std::move(value));
    }
    void delete_(T& value)
    {
        root = delete_node(root, value);
    }

    Node<T>* find(const T& value) const
    {
        return find_node(root.get(), value);
    }
    void in_order(const std::function<void(const T&)>& function) const
    {
        in_order_func(root, function);
    }
    void print_ascending() const
    {
        in_order_ascending(root.get());
    }
    void print_descending() const
    {
        in_order_descending(root.get());
    }
    
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
    FileReader(const std::string& file_name):file_name(file_name){ read_from_file(); }//file_name
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
            //std::getline(ss, seasons);

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

    void add_movie(const Movie& movie)
    {
        movies.add(movie);
    }
    void delete_movie()
    {

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
        movies.print_ascending();
    }
    void print_descending() const
    {
        movies.print_descending();
    }
    void end_session()
    {
        std::cout << "Thank you for using our services. We hope to se you again soon..." << std::endl;
        save_to_file();
    }
    void menu()
    {
        std::string number;
        std::cout << "Choose an action:\n [1] Sort movies A-Z \n [2] Sort movies Z-A \n [3] Search movie by title \n [4] Add a movie \n [5] Delete a movie [6] End session \n Choose a number from 1 to 4: ";
        std::cin >> number;

        while (number != "1" && number != "2" && number != "3" && number != "4" && number != "5")
        {
            std::cout << "Unknown action. Try again: "; std::cin >> number;
        }

        //while()
        switch (number[0])
        {
        case '1':
        {
            system("cls");
            print_ascending();
            break;
        }
        case '2':
        {
            system("cls");
            print_descending();
            break;
        }
        case '3':
        {
            system("cls");
            find_movie();
            break;
        }
        case '4':
        {
            std::string title, director;
            std::cout<<"Type movie's title : "; std::cin>>title;
            std::cout<<"Type director : "; std::cin>>director;

            add_movie(Movie(title, director));
            std::cout<<"Successfully added a movie " << title <<" by "<< director <<" to the library!"<<std::endl;
            print_ascending();
            end_session();
            break;
        }
        case '5':
        {
            std::string title;
            std::cout << "Find movie you want to delete by title : "; std::cin >> title;

            Movie temp_movie(title);
           // movies.delete_(temp_movie);
            std::cout << "Successfully deleted "<< temp_movie<<std::endl;
            print_ascending();
            break;
        }
        case '6':
        {
            end_session();
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


