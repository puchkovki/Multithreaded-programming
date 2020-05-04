#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "list.hpp"

struct input {
	const std::string& row;
	List< std::string >& list;
};

void Parallel_push_front(input in)  {
	in.list.push_front(in.row);
	in.list.push_back(in.row);
}

void fill_the_vector (std::vector< std::string >& song) {
	song.push_back("(Let him go!) ");
	song.push_back("Bismillah! No, we will not let you go. ");
	song.push_back("(Let him go!) ");
	song.push_back("Bismillah! We will not let you go. ");
	song.push_back("(Let him go!) ");
	song.push_back("Bismillah! We will not let you go. ");
	song.push_back("Easy come, easy go, will you let me go. ");
    song.push_back("Will not let you go. ");
	song.push_back("(Let me go.) ");
	song.push_back("Will not let you go. ");
	song.push_back("(Let me go.) ");
	song.push_back("Ah. ");
	song.push_back("No, no, no, no, no, no, no. ");
	song.push_back("(Oh mama mia, mama mia.) ");
	song.push_back("Mama mia, let me go. ");
	song.push_back("Beelzebub has a devil put aside for me! ");
	song.push_back("FOR ME! ");
	song.push_back("for me...");
}

int main(int argc, char** argv)
{
    List<std::string> list;
    
	if (argc < 2) {
		std::cout << "Not enough arguments";
	}
	size_t n_threads = 1;
	n_threads = atoll(argv[1]);
	
	std::vector<std::thread> threads;
	for(size_t i = 0; i < n_threads; i++) {
		input in = {"(Let him go!) ", list};
		//try {
		threads.push_back(std::thread(&Parallel_push_front, in));
		//} catch (const std::exception& e){
		//	std::cerr << e.what();
		//}
		//threads.push_back(std::thread(&Parallel_push_back, in));
	}
	for (size_t i = 0; i < n_threads; i++) {
		threads[i].join();
    }
	
    list.output();

    /*for (auto i: list) {
        std::cout << i.data << " ";
    }*/

    std::cout << std::endl;

    /////////////////////////////////////////////////////////

    /*List<int> list_int;
    list_int.push_front(1941);
    list_int.push_back(1945);

    //list_int.display();
    if (list_int.begin() != list_int.end()) {
        std::cout << list_int.begin();
    }*/
    
	return 0;
}