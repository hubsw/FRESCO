#ifndef THREADTOOLS_H
#define THREADTOOLS_H


template<typename Data>
class concurrent_queue
{
private:
    queue<Data> the_queue;
    mutable boost::mutex the_mutex;
    mutable boost::mutex the_mutex2;
    boost::condition_variable the_condition_variable;

    long finished;
    long prodcount;
    unsigned long maxSize;
    string queueName;

public:
    concurrent_queue(long _prodcount, unsigned long _maxSize, string _queueName) { finished=0; prodcount=_prodcount; maxSize=_maxSize; queueName=_queueName;}

    /*void push(Data const& data)
    {
        boost::mutex::scoped_lock lock(the_mutex);
        the_queue.push(data);
        lock.unlock();
        the_condition_variable.notify_one();
    }*/

    void push(Data const& data)
    {
        boost::mutex::scoped_lock lock(the_mutex);

        while (the_queue.size()>maxSize)
        {
            cout << "Queue " << queueName << " Full\n";
            boost::thread::sleep(boost::get_system_time()+ boost::posix_time::milliseconds(500));
        }

        the_queue.push(data);
        lock.unlock();
        the_condition_variable.notify_one();
    }

    bool empty() const
    {
        boost::mutex::scoped_lock lock(the_mutex2);
        return the_queue.empty();
    }

    bool wait_and_pop(Data& popped_value)
    {
        boost::mutex::scoped_lock lock(the_mutex2);
        while(the_queue.empty() and !(finished==prodcount))
        {
            boost::system_time const timeout=boost::get_system_time()+ boost::posix_time::milliseconds(250);
            the_condition_variable.timed_wait(lock, timeout);
        }
        if (the_queue.empty() and (finished==prodcount))
        {
            return false;
        }else
        {
            popped_value=the_queue.front();
            the_queue.pop();
        }
        return true;
    }

    void finish() { finished++; }

};

class ThreadFramework
{
    public:
        void addThread(boost::thread* t) { threads.push_back(t); }
        void runAll() { BOOST_FOREACH(boost::thread* t, threads) t->join(); }

    private:
        list<boost::thread*> threads;
};

class MyTemplateThread
{
	public:
	virtual void run() {};
};



class MyTemplateFunctionThread: public MyTemplateThread
{
    public:

    MyTemplateFunctionThread(long (*_nFunc)(string), string _par)
    {
        nFunc=_nFunc;
        par=_par;
    }

    virtual ~MyTemplateFunctionThread()
    {

    }

	void run()
	{
        nFunc(par);
	};

    string par;
	long (*nFunc)(string par1);
};

class ThreadTools
{
    public:
        ThreadTools() {}

        static void runAllThreads(std::vector<MyTemplateThread*> *userthreads, int maxThreads, bool print=true)
        {
            std::vector<boost::thread*> threads;
            //std::vector<boost::thread*>::size_type a=userthreads->size();

            for (int i=0;i<maxThreads;i++)
                threads.push_back(NULL);

            std::vector<boost::thread*>::size_type current=0;

            //Perform job

            if (print)
                cout << "  Processing " << boost::lexical_cast<std::string>(userthreads->size()) << " jobs:\n";
            StopWatch sw;
            while (current<userthreads->size())
            {
                for (std::vector<boost::thread*>::size_type ti=0;ti<threads.size();ti++)
                {
                    if (current<userthreads->size())
                    {
                        if (threads[ti]==NULL)
                        {

                            if (print)
                            {
                                sw.stop(false);
                                long eta=(sw.getDiff()*userthreads->size()/(current+1)-sw.getDiff())/1000;
                                string etas="?";
                                if (eta!=0) etas=boost::lexical_cast<string>(eta);
                                cout << "Job "<<boost::lexical_cast<std::string>(current+(std::vector<boost::thread*>::size_type)1)<<", ETA="<<etas<< " seconds\n" << std::flush;
                            }
                            threads[ti]=new boost::thread(boost::bind(&MyTemplateThread::run, userthreads->at(current++)));
                        }else if(threads[ti]->timed_join(boost::posix_time::microseconds(10)))
                                threads[ti]=NULL;
                    }
                }
            }

            //cout << "\n";
            if (print)
                cout << "Finished!\n";

            //Clean up)

            for (std::vector<boost::thread*>::size_type ti=0;ti<threads.size();ti++)
                if (threads[ti]!=NULL)
                    threads[ti]->join();

        }




        static void runAllFunctions(int maxThreads, long (*nFunc)(string), vector<string> &pars)
        {
            std::vector<MyTemplateThread*> userthreads;

            BOOST_FOREACH(string s, pars)
            {
                userthreads.push_back(new MyTemplateFunctionThread(nFunc, s));
            }

            runAllThreads(&userthreads, maxThreads, false);


            BOOST_FOREACH(MyTemplateThread *t, userthreads)
                delete (MyTemplateFunctionThread*)t;

        }

    protected:
};

#endif // THREADTOOLS_H
