#pragma once

#include "inc.h"

namespace writr
{
    // Class for storing various statuses
    template<class T>
    class Status
    {
        T status;

    protected:
        Status() {}
        Status(T s) : 
            status(s)
        {

        }

        static std::vector<std::string> &getValues()
        {
            static std::vector<std::string> values;

            return values;
        }

        void setStatus(T s)
        {
            status = s;
        }

    public:
        int getStatusNum() const
        {
            return (int)status;
        }

        std::string getStatusValue() const
        {
            return getValues()[getStatusNum()];
        }
    };
}