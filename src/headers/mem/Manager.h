#pragma once

#include "inc.h"
#include "Data/Singleton.h"

namespace writr
{
    template<typename T>
    class Manager 
    {
        std::vector<T*> objects;

    public:
        static Manager<T> &get()
        {
            static Manager<T>* m;

            if (!m)
            {
                m = new Manager<T>();
            }

            return *m;
        }

        Manager()
        {
            objects.reserve(10);
        }

        T* operator () (unsigned int index)
        {
            return objects.at(index);
        }

        T* push(T* obj)
        {
            objects.push_back(obj);
            return objects.at(objects.size() - 1);
        }

        T &push(T obj)
        {
            objects.push_back(new T(obj));
            return objects.at(objects.size() - 1);
        }
    };
}