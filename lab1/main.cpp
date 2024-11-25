#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

mutex mutex_;
condition_variable conditional_var;
bool flag = false;

void f_producer();
void f_consumer();


int main()
{
	thread producer(f_producer);
	thread consumer(f_consumer);
	producer.join();
	consumer.join();
}


void f_producer()
{
    while (true)
    {
        this_thread::sleep_for(chrono::seconds(1));
        {
            unique_lock<mutex> unique_l(mutex_);
            if (flag) // check потерялся ли флаг
            {
                cout << "Warning: Event lost!" << endl;
            }
            else
            {
                flag = true;
                cout << "Event sent" << endl;
                conditional_var.notify_one();
            }
        }
    }
}


void f_consumer()
{
	while (true)
	{
		unique_lock<mutex> unique_l(mutex_);
		conditional_var.wait(unique_l, []() {return flag; });
		flag = false;
		cout << "Event processed" << endl;
	}
}