#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

int counter = 0;
std::mutex ctMutex;

void incrementCounter() 
{
    for(int i = 0; i < 10000; ++i) 
    {
        std::lock_guard<std::mutex> guard(ctMutex); // different threads are doing different work.
        ++counter;
        std::cout<<counter<<'\n';
    }
}

int main() 
{
    std::vector<std::thread> threads;
    
    for(int i = 0; i < 10; ++i) 
    {
        threads.emplace_back(incrementCounter);
    }
    
    for(auto &t : threads) 
    {
        t.join(); // this just waits for the threads to complete their work!
    }
    
    std::cout << "Counter = " << counter << "\n";  
    return 0;
}
