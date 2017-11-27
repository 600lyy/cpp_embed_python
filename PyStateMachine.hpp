#ifndef PyStateMachine_H_
#define PyStateMachine_H_

#include <boost/python.hpp>
#include <string>
#include <assert.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "worktaskParameterSet.hpp"
#include "workTaskCmd.hpp"


#define PY_STATEMACHINE_NS_BEGIN namespace PyStateMachineNS {
#define PY_STATEMACHINE_NS_END }

PY_STATEMACHINE_NS_BEGIN

using WorkTaskParameterSetImp::ParameterSet;
using WorkTaskParameterSetImp::WorkTaskParameterSet;
using NS_WorkTaskCmdImp::WorkTaskCmdBase;
using NS_WorkTaskCmdImp::WorkTaskCmdPsetCreated;

enum IOEventID
{
    ROMPT_SOCKET = 0,
    BARCODE_SCANNER_INPUT = 1,
    SOCKET_LIFTED = 2,
    SELECTED_PSET_ID = 3,
    HMI_SELECT_VALUE = 4,
};

struct TestClass {

    TestClass():mValue(0)
    {}

    inline
    int getValue() const
    {
        return mValue;
    }

    inline
    void setValue(std::size_t value)
    {
        mValue = value;
    }
    
    std::size_t mValue;

};


class PyStateMachine {

    public:

        PyStateMachine() : 
            mPyScriptPath("/home/autotest/embed_python/"), mPyModule(), 
            mWtps(NULL), mPyScriptName(), mPyScriptFullName(), mTest(6),
            mWtcd(NULL), mFirstLaunch(true)
        {}

        ~PyStateMachine() 
        {
            
        }

        void load(std::shared_ptr<ParameterSet>&, int);

        void reinit();

        inline void initPy();

        void funcNewWorkTaskMsg(std::shared_ptr<WorkTaskCmdBase>&); 

        const std::shared_ptr<ParameterSet>& getTaskParameter() const; 

        void setTaskParameter(std::shared_ptr<ParameterSet>& );

        void forwardDone(int, std::shared_ptr<ParameterSet>&);

        void forwardEvent(const std::string& event);

        std::string GetCurrentState() const;

        std::string __tostring() const
        {
            return "<PyStateMachine (" + std::to_string(mTest) + ")>";
        }

        inline
        void getTestClassValue(TestClass* pTest)
        {
            assert(pTest);
            std::cout<< "Value of TestClass is " << pTest->getValue() <<std::endl;
        }

        inline
        void setTestClassValue(TestClass* pTest, size_t value)
        {
            assert(pTest);
            pTest->setValue(value);
        }

        inline
        void funcSendIOXSignal(IOEventID eventid, int value)
        {
            std::cout << "In Cpp Send IOX event with Id: " << eventid 
                << ", and hmi select id is "<< value <<std::endl;
        }

        inline
        void funcWorktaskDone()
        {
            std::cout << "WorkTaskDone in CPP" << std::endl;
        }

        inline
        void funcSubscribeIOXSignal(IOEventID eventid, std::string event)
        {
            std::cout <<"In cpp funcfuncSubscribeIOXSignal" << event << std::endl;
        }

    public:

        boost::python::object mPyModule;

        boost::python::object mRunscript;

        boost::python::object mPyNameSpace;

        boost::python::object mMainNameSpace;

        std::shared_ptr<ParameterSet> mWtps;

        std::shared_ptr<WorkTaskCmdBase> mWtcd;

        int mTest;

        bool mFirstLaunch;

        std::string mPyScriptPath;

        std::string mPyScriptName;

        std::vector<std::string> mImportedScripts;

        std::string mPyScriptFullName;

};

BOOST_PYTHON_MODULE(cppwrapper)
{
    namespace python = boost::python;
    python::class_<TestClass>("TestClass")
        .def("getValue", &TestClass::getValue)
        ;
    python::class_<PyStateMachine>("PyStateMachine")
        .def("funcSendIOXSignal", &PyStateMachine::funcSendIOXSignal)
        .def("funcWorktaskDone", &PyStateMachine::funcWorktaskDone)
        .def("funcSubscribeIOXSignal", &PyStateMachine::funcSubscribeIOXSignal)
        ;
    python::class_<ParameterSet>("ParameterSet")
        .def("test_func", &ParameterSet::test_func)
        .def("class_name", &ParameterSet::name)
        ;
    python::class_<WorkTaskParameterSet, python::bases<ParameterSet> >("WorkTaskParameterSet")
        .def("set_work_task", &WorkTaskParameterSet::setWorktask)
        .def("class_name", &WorkTaskParameterSet::name)
        .def("get_work_task", &WorkTaskParameterSet::getWorktask,
                python::return_value_policy<python::copy_const_reference>())
        ;
    python::class_<WorkTaskCmdBase, boost::noncopyable>("WorkTaskCmdBase", python::no_init)
        ;
    python::class_<WorkTaskCmdPsetCreated, std::shared_ptr<WorkTaskCmdPsetCreated>, python::bases<WorkTaskCmdBase> >("WorkTaskCmdPsetCreated")
        .def("class_name", &WorkTaskCmdPsetCreated::_name)
        .def_readwrite("mIndex", &WorkTaskCmdPsetCreated::mIndex)
        ;
    python::enum_<IOEventID>("IOEventID")
        .value("SOCKET_LIFTED", IOEventID::SOCKET_LIFTED)
        .value("BARCODE_SCANNER_INPUT", IOEventID::BARCODE_SCANNER_INPUT)
        .value("SELECTED_PSET_ID", IOEventID::SELECTED_PSET_ID)
        ;

    python::register_ptr_to_python<std::shared_ptr<WorkTaskCmdBase> >();

    python::scope().attr("__doc__") = "cppwrapper Module";
}

const 
std::shared_ptr<ParameterSet>& PyStateMachine::getTaskParameter() const 
{

    std::cout << "In Cpp: PyStateMachine::getTaskParameter() is called" << std::endl; 
    return mWtps;
}

void PyStateMachine::funcNewWorkTaskMsg(std::shared_ptr<WorkTaskCmdBase>& TaskPtr) 
{

    mPyModule.attr("new_work_taskmsg")(TaskPtr);

    //std::string name = boost::python::extract<std::string>(mPyModule.attr("my_name"));
    
}

void PyStateMachine::load(std::shared_ptr<ParameterSet>& pWtps, int instance) 
{

    namespace py = boost::python;
    mWtps = pWtps;

    mPyScriptName = pWtps->getWorktask();
    mPyScriptFullName = mPyScriptPath + mPyScriptName;
    std::cout << "The python script to be executed is " + mPyScriptFullName << std::endl; 

    //mPyModule = PyImport_Import(PyUnicode_DecodeFSDefault("test1"));
    //
    initPy();

    mPyModule = py::import(mPyScriptName.c_str());
    mPyNameSpace = mPyModule.attr("__dict__");

    py::scope(mRunscript).attr("machine") = mPyModule.attr("model");

    mMainNameSpace[mPyScriptName.c_str()] = mPyModule; 
    mMainNameSpace["runscript"] = mRunscript; 

}

void PyStateMachine::reinit() 
{
}

inline
void PyStateMachine::initPy() 
{
    namespace py = boost::python;

    if (mFirstLaunch == true) {

        PyImport_AppendInittab("cppwrapper", &PyInit_cppwrapper);
        setenv("PYTHONPATH","/home/autotest/embed_python",1);
        Py_Initialize();
        mMainNameSpace = py::import("__main__").attr("__dict__");
        py::exec("from importlib import reload", mMainNameSpace);
        py::exec("import gc", mMainNameSpace);
        mRunscript = py::import("runscript");
        py::scope(mRunscript).attr("statemachine_interface") = this;

        mImportedScripts.push_back(mPyScriptName);
        mFirstLaunch = false;
    }
    else {

        py::object RunscriptNameSpace = mRunscript.attr("__dict__");
        py::exec("del machine", RunscriptNameSpace);
        py::exec("gc.collect()", RunscriptNameSpace);
        py::exec("first_run = True", RunscriptNameSpace);

        if (std::find(mImportedScripts.begin(), mImportedScripts.end(), mPyScriptName) != mImportedScripts.end()) {
            std::cout << "Reload python module " << mPyScriptName << std::endl;
            std::string command = std::string("reload(") + mPyScriptName + std::string(")");
            py::exec(command.c_str(), mMainNameSpace);
        }
        else {
            mImportedScripts.push_back(mPyScriptName);
        }

        py::exec("gc.collect()", mMainNameSpace);
    }
    //setenv("PYTHONWARNINGS","ignore",1);
}

void PyStateMachine::forwardDone(int runId, std::shared_ptr<ParameterSet>& pWtps) 
{

}

std::string PyStateMachine::GetCurrentState() const 
{
    std::string currentState = boost::python::extract<std::string>(mPyModule.attr("get_state")());
    return currentState;
} 

void PyStateMachine::setTaskParameter(std::shared_ptr<ParameterSet>& pWtps) 
{

    mWtps = pWtps;
}

void PyStateMachine::forwardEvent(const std::string& event)
{
    mPyModule.attr("send_event")(event);
}

PY_STATEMACHINE_NS_END

#endif
