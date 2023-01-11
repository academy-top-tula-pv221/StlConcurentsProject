#include <iostream>
#include <thread>
#include <mutex>
#include <future>

using namespace std;

int counter{};
mutex m;

void ParallelFunc1(int n)
{
    for (int i = 0; i < n; i++)
    {
        lock_guard<mutex> gm(m);
        cout << "\t" << this_thread::get_id() << " sub thread: " << i << "\n";
    }  
}

void ParallelFunc2(int a, int n)
{

    for (int i = a; i < n; i++)
    {
        lock_guard<mutex> gm(m);
        cout << "\t" << this_thread::get_id() << " sub thread: " << i << "\n";
    }

}

void Counter()
{
    for (int i = 0; i < 10000; i++)
    {
        m.lock();
        counter++;
        m.unlock();
    }
        
}

int Accumulator(int n)
{
    int a{};
    for (int i = 1; i <= n; i++)
    {
        a += i;
        this_thread::sleep_for(10ms);
    }  
    return a;
}

void AccumulatorPromise(int n, promise<int> p)
{
    int a{};
    for (int i = 1; i <= n; i++)
    {
        a += i;
        this_thread::sleep_for(10ms);
    }
    p.set_value(a);
}

int main()
{
    /*
    thread th1(ParallelFunc1, 450);
    thread th2(ParallelFunc2, 100, 500);

    for (int i = 0; i < 400; i++)
    {
        m.lock();
        cout << this_thread::get_id() << " main thread: " << i << "\n";
        m.unlock();
    }
        

    th1.join();
    th2.join();

    cout << counter;

    */

    /*
    future<int> fres1 = async(Accumulator, 500);
    future<int> fres2 = async(Accumulator, 400);

    //fres1.wait();
    //fres2.wait();

    auto v1 = async([&fres1]()
        { 
            cout << "res 1 = " << fres1.get() << "\n"; 
        });
    auto v2 = async([&fres2]()
        {
            cout << "res 2 = " << fres2.get() << "\n";
        });

    cout << "main working...\n";

    promise<int> promise1;
    promise<int> promise2;
    future<int> fp1 = promise1.get_future();
    future<int> fp2 = promise2.get_future();

    thread ath1(AccumulatorPromise, 100, move(promise1));
    thread ath2(AccumulatorPromise, 50, move(promise2));

    auto v3 = async([&fp1]()
        {
            cout << "res 3 = " << fp1.get() << "\n";
        });
    auto v4 = async([&fp2]()
        {
            cout << "res 4 = " << fp2.get() << "\n";
        });

    ath1.join();
    ath2.join();
    */

    packaged_task<int(int)> task1(Accumulator);
    future<int> taskResult1 = task1.get_future();
    packaged_task<int(int)> task2(Accumulator);
    future<int> taskResult2 = task2.get_future();

    packaged_task<void()> taskCout1([&taskResult1]() {
        cout << this_thread::get_id() << " task 1 = " << taskResult1.get() << "\n";
        });
    packaged_task<void()> taskCout2([&taskResult2]() {
        cout << this_thread::get_id() << " task 2 = " << taskResult2.get() << "\n";
        });

    thread t1(move(task1), 300);
    thread t2(move(task2), 100);

    auto v1 = async([&taskResult1]()
        {
            cout << "res 1 = " << taskResult1.get() << "\n";
        });
    auto v2 = async([&taskResult2]()
        {
            cout << "res 2 = " << taskResult2.get() << "\n";
        });

    t1.join();
    t2.join();

    //taskCout1();
    //taskCout2();
    
}
