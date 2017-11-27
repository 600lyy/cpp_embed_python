#ifndef WorkTaskRunner_H_
#define WorkTaskRunner_H_

#include "PyStateMachine.hpp"
#include <iostream>
#include <string>
#include "worktaskParameterSet.hpp"

using namespace WorkTaskParameterSetImp;
using namespace PyStateMachineNS;
using namespace NS_WorkTaskCmdImp;

namespace WorkTaskRunnerImp {

    bool doLoadWorktask(PyStateMachine* pState, 
            std::shared_ptr<ParameterSet>& pCurrentWtpsObject) {

        if (!pState) {
            std::cout << "PyStateMachine is NULL" << std::endl;
            return false;
        }   

        std::shared_ptr<WorkTaskCmdBase> newTask(new WorkTaskCmdPsetCreated());

        try 
        {
            pState->load(pCurrentWtpsObject, 1);
            //pState->funcNewWorkTaskMsg(newTask);
        }
        catch (const boost::python::error_already_set&)
        {
            PyErr_Print();
        }

        return true; 
    }
} //namespace

#endif

