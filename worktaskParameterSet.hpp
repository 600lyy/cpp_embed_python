#ifndef WorkTaskParameterSet_H_
#define WorkTaskParameterSet_H_

#include <iostream>
#include <memory>

namespace WorkTaskParameterSetImp {

    struct ParameterSet {

        std::string mTaskName;

        inline 
        ParameterSet():mTaskName("noTask")
        {}

        inline
        ParameterSet(const std::string& task):mTaskName(task)
        {}

        const std::string& getWorktask() const 
        {
            return mTaskName;
        }

        void setWorktask(const std::string& task)
        {
            mTaskName = task;
        }

        void test_func()
        {
            std::cout <<"ParameterSet::test_func() is called" << std::endl;
        }

        inline
        std::string __tostring() const
        {
            return "<ParameterSet>"; 
        }

        inline
        virtual std::string name() const
        {
            return "ParameterSet";
        }

        virtual ~ParameterSet()
        {}
    };


    struct WorkTaskParameterSet : ParameterSet {

        inline 
        WorkTaskParameterSet()
        {}

        inline
        WorkTaskParameterSet(const std::string& task):
            ParameterSet(task)
        {}

        virtual const std::string& getWorktask() const {
            return mTaskName;
        }

        virtual void setWorktask(const std::string& task) {
            mTaskName = task;
        }
        inline
        virtual std::string name() const
        {
            return "WorkTaskParameterSet";
        }
        
    };
}//WorkTaskParameterSetImp

#endif

