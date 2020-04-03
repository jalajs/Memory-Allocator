#include libraries here...

//
some_thread1(int index_to_shared_data){
	// Allocates some memory for an individual string
	
	// Generate some random data/string to put in shared_data
	// e.g. shared_data[index_to_shared_data] "some_thread1";
	
	// Terminate thread
}

some_thread2(int index_to_shared_data){
	// Allocates some memory for an individual string
	
	// Generate some random data/string to put in shared_data
	// e.g. shared_data[index_to_shared_data] "some_thread2";
	
	// Terminate thread
}

// Here is some data structure that will be shared.
// It is an array of strings, but nothing has been allocated yet.
char** shared_data

int main(){
	// (1) Malloc for some size of your shared data (i.e. how many strings will you have)

	// (2) Launch some number of threads (perhaps with two or more different functions)

	// (3) Join some number of threads
	
	// (4) Print the results of shared data (i.e. this is done sequentially)
	
	// (5) Cleanup your program
	// 'free shared_data'
}
