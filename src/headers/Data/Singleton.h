#pragma once

namespace writr
{
    template<typename T>
    struct Singleton
    {
        static T &get()
        {
            static T* t;

            if (!t)
            {
                t = new T();
            }

            return *t;
        }
    };
}