#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <condition_variable>
#include <mutex>

using std::cout;
using std::endl;

class thread_guard{
    std::thread &t;
    
public:
    explicit thread_guard(std::thread& t_): t(t_){}
    thread_guard(const thread_guard & other) = delete;
    thread_guard& operator=(const thread_guard & other) = delete;
    
        // Destructor
    ~thread_guard(){
        cout << "Destructing" << endl;
        if(t.joinable()) t.join();
    }
};

/*
 *  ================ Examples ================
 *
 *  std::thread t1{myFunc};
 *  jthread jt1(myFunc);
 *  jthread jt2(std::move(t1));
 *  jthread jt3{std::thread(myFunc)};
 *  jthread jt4 = std::move(jt2);
 *  jthread jt5 = jthread(myFunc);
 *  jthread jt6 = jthread(std::thread(myFunc));
 */

class jthread{
    std::thread t;
    
public:
    
    jthread() noexcept = default;
    
    template<typename Callable, typename ... Args>
    explicit jthread(Callable && func, Args&& ... args):
    t(std::forward<Callable>(func), std::forward<Args>(args)...){}
    
    explicit jthread(std::thread t_):t(std::move(t_)){}
    
    jthread(jthread&& other) noexcept:t(std::move(other.t)){}
    
    jthread& operator=(jthread && other) noexcept{
        if(joinable()) join();
        t = std::move(other.t);
        return *this;
    }
    
    jthread& operator=(std::thread other) noexcept{
        if(joinable()) join();
        t = std::move(other);
        return *this;
    }
    
    void swap(jthread& other) noexcept{
        this->t.swap(other.t);
    }
    
    std::thread::id get_id() const noexcept{
        return t.get_id();
    }
    
    bool joinable() const noexcept{
        return t.joinable();
    }
    
    void join(){
        t.join();
    }
    
    void detach(){
        t.detach();
    }
    
    std::thread& as_thread() noexcept{
        return t;
    }
    
    const std::thread& as_thread() const noexcept{
        return t;
    }
    
    ~jthread(){
        if(t.joinable()) t.join();
    }
};

void myFunc(int x){
    for(int i = 0; i < x; i++){
        cout << "Thread" << i << endl;
    }
}

void main_process(){
    int x = 1;
    int y = 0;
    for(int i = 0; i < 10; i++){
        cout << "Main" << i << endl;
    }
}

int main(){
    jthread t1{myFunc, 5};
    return 0;
}
