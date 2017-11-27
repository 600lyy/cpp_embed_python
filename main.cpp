#include <chrono>
#include <thread>
#include "PyStateMachine.hpp"
#include "worktaskrunner.hpp"
#include <iostream>
#include <exception>
//#include <boost/shared_ptr.hpp>
//#include <boost/make_shared.hpp>

using namespace WorkTaskRunnerImp;
using namespace PyStateMachineNS;

std::string parse_python_exception() 
{
    namespace py = boost::python;

    PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
    PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
    std::string ret("Unfetchable Python error");
    if(type_ptr != NULL) {
        py::handle<> h_type(type_ptr);
        py::str type_pstr(h_type);
        py::extract<std::string> e_type_pstr(type_pstr);
        if(e_type_pstr.check()) {
            ret = e_type_pstr();
        }
        else {
            ret = "Unknown exception type";
        }
    }
    if(value_ptr != NULL) {
        py::handle<> h_val(value_ptr);
        py::str a(h_val);
        py::extract<std::string> returned(a);
        if(returned.check())
            ret +=  ": " + returned();
        else
            ret += std::string(": Unparseable Python error: ");
    }
    if(traceback_ptr != NULL){
        py::handle<> h_tb(traceback_ptr);
        py::object tb(boost::python::import("traceback"));
        py::object fmt_tb(tb.attr("format_tb"));
        py::object tb_list(fmt_tb(h_tb));
        py::object tb_str(py::str("\n").join(tb_list));
        py::extract<std::string> returned(tb_str);
        if(returned.check())
            ret += ": " + returned();
        else
            ret += std::string(": Unparseable Python traceback");
    }

    return ret;
}

int main(int argc, char *argv[])
{
    PyStateMachine* state_machine = new PyStateMachine();

    std::shared_ptr<ParameterSet> pWtps(std::make_shared<WorkTaskParameterSet>("hmi_tightening"));

    try
    {
        doLoadWorktask(state_machine, pWtps);
        state_machine->forwardEvent("wte_Start");

        unsigned int count = 0;

        while(count <= 3) {

            std::string currentState = state_machine->GetCurrentState();

            if (currentState.c_str() == "TighteningBlock_HmiCancelTighteningBlock_id_3") {

                std::cout << "WorkTaskRunner is now terminating..." << std::endl;
                break;
            }

            std::cout << "current state is: " << currentState.c_str() << " waiting for events to trigger transistion"
                << std::endl;

            count++;

            switch (count) {

                case 2: {
                    state_machine->forwardEvent("internal_PrimitiveDone");
                    break;
                }
                case 4: {
                    state_machine->forwardEvent("internal_HmiSelect");
                    break;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }

        //Py_Finalize();
    }
    catch(...)
    {
        std::exception_ptr p_err = std::current_exception();
        std::cout <<(p_err ? p_err.__cxa_exception_type()->name() : "null") << std::endl;
    }
       //pWtps.reset();

    //std::cout<< "Reference count of Wtps is " << pWtps.use_count() << std::endl;
    //

    for(int i=0;i<=20000;++i) {
        if (i%2 == 0) {
            pWtps->setWorktask("test_tightening");
        }
        else {
            pWtps->setWorktask("hmi_tightening");
        }
        doLoadWorktask(state_machine, pWtps);
        state_machine->forwardEvent("wte_Start");
    }

    delete state_machine;
    state_machine = NULL;

    return 0;
}
