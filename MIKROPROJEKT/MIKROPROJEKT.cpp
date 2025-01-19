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

    bool operator <(const Movie& other_movie)
    {
        return title < other_movie.title;
    }
    bool operator >(const Movie& other_movie)
    {
        return title > other_movie.title;
    }
    bool operator ==(const Movie& other_movie)
    {
        return title == other_movie.title;
    }   
    friend std::ostream& operator<<(std::ostream& COUT, const Movie& movie);

    std::string getGenre() const { return genre; }
    std::string getTitle() const { return title; }
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

    void add_node(std::unique_ptr<Node<T>>& current, T value)
    {
        if (!current)
            current = std::make_unique<Node<T>>(std::move(value));
        else if (value < current->data)
            add_node(current->left, value);
        else if (value > current->data)
            add_node(current->right, value);
        else
        {}
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

    void add(T value)
    {
        add_node(root, std::move(value));
    }

    Node<T>* find(const T& value) const
    {
        return find(root.get(), value);
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
/*
template<typename T>
class Element
{
public:
    T data;
    std::shared_ptr<Element<T>> next;

    Element(T data) : data(data), next(nullptr){}
};
template<typename T>
class List
{
private:
   std::shared_ptr<Element<T>> head, tail;
   
public:
    size_t size;
    List() : head(nullptr),tail(nullptr),size(0) {}
    List(const List<T>& other_list) : head(nullptr), tail(nullptr), size(0) 
    {
        auto current_element = other_list.head;
        while (current_element)
        {
            push_back(current_element->data);
            current_element = current_element->next;
        }
    }
    //List(const &List)
    
    void push_front(const T& data)
    {
        auto new_element = std::make_shared<Element<T>>(data);

        if (head) //if(head!=nullptr)       
            new_element->next = head;       
        else
            tail = new_element;

        head = new_element;
        size++;
    }
    void push_back(const T& data)
    {
        auto new_element = std::make_shared<Element<T>>(data);

        if (!head) //if(head==nullptr)        
           head = tail = new_element;
        else
        {
            tail->next = new_element;  
            tail = new_element;
        }
        size++;
    }
    void print()
    {
        if (!head)
        {
            std::cout << "List is empty" << std::endl;
            return;
        }
        auto current_element = head;

        //std::cout << "[ ";
        while (current_element)
        {
            std::cout << current_element->data << " ";
            current_element = current_element->next;
        }
        //std::cout << "]"<<std::endl;
    }     
    
    List<T>& operator =(List<T>& other_list)
    {
        if (this != other_list)
        {
            clear();
            auto current_element = other_list.head;
            while (current_element)//!= nullptr
            {
                push_back(current_element->data);
                current_element = current_element->next;
            }
        }
        return *this;
    }
    

    class Iterator
    {
    private:
        std::shared_ptr<Element<T>> current;
    public:
        Iterator(std::shared_ptr<Element<T>> start):current(start){}

        T& operator*() { return current->data; }
        T* operator->() { return &(current->data); }
        Iterator& operator++()
        {
            if (current) //current != nullptr
                current = current->next;
            return *this;
        }
        bool operator ==(const Iterator& other) const { return current == other.current; }
        bool operator !=(const Iterator& other) const { return current != other.current; }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }

    void clear()
    {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    ~List()
    {
        clear();
    }
};
template<typename Iterator, typename T>
Iterator find(Iterator begin, Iterator end, const T& value)
{
    for (auto it = begin; it != end; ++it)
    {
        if (*it == value)
            return it;
    }
    return end;
}
template<typename Iterator, typename Predicate>
Iterator find_if(Iterator begin, Iterator end, Predicate pred)
{
    for (auto it = begin; it != end; ++it)
    {
        if (pred(*it))
            return it;
    }
    return end;
}
*/
bool validate_data(const std::string& title, const std::string& director,
    const int& year, const double& rating, const std::string& genre)
{ 
    return !(title.empty()|| director.empty() || genre.empty() ||year>2024||rating>10.0||rating<0.0);
}

class FileReader
{
private:
    FileReader(const std::string& file_name) { read_from_file(file_name); }
    FileReader(const FileReader&) = delete;
    FileReader operator=(const FileReader&) = delete;

    BinaryTree<Movie> movies, favourites;

    void read_from_file(const std::string& file_name)
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
            std::getline(ss, seasons);

            try
            {
                int year = std::stoi(release_year);
                double rat = std::stod(rating);

                if (validate_data(title, director, year, rat, genre))
                    //movie_data.push_back(Movie(title, director, year, rat, genre));
                    movies.add(Movie(title, director, year, rat, genre));
            }
            catch (const std::exception except)
            {
                std::cout << "Unable to properly read data from line: " << line <<"Exception - "<<except.what() << std::endl;
            }           
        }
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
        //zapisywanie itd
    }
    void menu()
    {
        std::string number;
        std::cout << "Choose an action:\n [1] Sort movies A-Z \n [2] Sort movies Z-A \n [3] Search movie by title \n [4] End session \n Choose a number from 1 to 4: ";
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
            end_session();
        }
        default:
            break;
        }        
    }
};

class Interface
{
private:
    
public:
    BinaryTree<Movie> movies;

    Interface(){}   
};

int main()
{
  /*  List<char> lista_char;
    std::string zeniucha{ "zeniucha" };
    
    auto it = find(lista_char.begin(), lista_char.end(), 'z');
    if (it!=lista_char.end())
        std::cout << "Found " << *it << std::endl; 
   try
    {
        FileReader& file_reader = FileReader::getInstance("movies.txt");
        List<Movie> movies = file_reader.getDataList();

        auto it = find_if(movies.begin(), movies.end(), [](Movie movie) {return movie.getGenre() == "Action"; });
        if(it != movies.end())
            std::cout << *it;
    }
    catch (const std::exception& except)
    {
        std::cout << except.what() << std::endl;
    }
*/
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


