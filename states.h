#ifndef STATES_H
#define STATES_H

#include "executer.h"
#include <ostream>
#include <sstream>


inline static std::ostream* stream_ptr=&(std::cout);

//Интерфейс состояний
class IState: public BaseState
{

public:
    IState(const Executor_ptr exec) : m_executor(exec) {}
    virtual void execute(std::string) override {};
    Executor_ptr get_executor() {return m_executor;}
    size_t getSize() {return m_executor.lock()->getExecutorData().size;}


    virtual ~IState()=default;
protected:
    void print_block()
    {
        auto command_list=m_executor.lock()->getCommandList();
        if (!command_list->empty())
        {
            *(stream_ptr)<<"bulk: "<<command_list->front();
            command_list->pop_front();
        }
        for (auto x : *command_list) *(stream_ptr)<<", "<<x;
        *(stream_ptr)<<std::endl;
        m_executor.lock()->getCommandList()->clear();
    }
private:
    Executor_ptr m_executor;


};





//Состояние 1
//Для сохранения неизменности executor исходное состояние должно называться "FIRST_STATE"
class Simple_Commamd_Queue_State: public IState
{

public:
    Simple_Commamd_Queue_State(const Executor_ptr& exec ) : IState(exec)  {}
    inline const static std::string id="FIRST_STATE";
    virtual void execute(std::string command) final override
    {
        if(command=="EOF")
        {
            print_block() ;
            return;
        }
        if (command=="{")
        {
            print_block();
            get_executor().lock()->setState("DYNAMIC_COMMAND_QUEUE");
            return;
        }
        auto list=get_executor().lock()->getCommandList();
        list->push_back(command);
        if (list->size()==getSize()) print_block();
    };
    ~Simple_Commamd_Queue_State()=default;
};

//Состояние 2
class Dynamic_Commamd_Queue_State: public IState
{

public:
    Dynamic_Commamd_Queue_State(const Executor_ptr& exec ) : IState(exec)  {}
    inline const static std::string id="DYNAMIC_COMMAND_QUEUE";
    virtual void execute(std::string command) final override
    {
        if(command=="EOF")
        {
            get_executor().lock()->getCommandList()->clear();
            get_executor().lock()->setState("FIRST_STATE");
            return;
        }

        if(command=="}")
        {
            print_block();
            get_executor().lock()->setState("FIRST_STATE");
            return;
        }
        if(command=="{")
        {
            get_executor().lock()->setState("DYNAMIC_COMMAND_QUEUE_NESTED_BLOCK");
            return;
        }
        auto list=get_executor().lock()->getCommandList();
        list->push_back(command);
    };
    ~Dynamic_Commamd_Queue_State()=default;
};

//Состояние 3
class Dynamic_Commamd_Queue_Nested_Block_State: public IState
{
    size_t m_count=1;
public:
    Dynamic_Commamd_Queue_Nested_Block_State(const Executor_ptr& exec ) : IState(exec)  { }
    inline const static std::string id="DYNAMIC_COMMAND_QUEUE_NESTED_BLOCK";
    virtual void execute(std::string command) final override
    {
        if(command=="EOF")
        {
            get_executor().lock()->getCommandList()->clear();
            return;
        }

        if(command=="}")
        {
            m_count--;
            if (m_count==0) get_executor().lock()->setState("DYNAMIC_COMMAND_QUEUE");
            return;
        }
        if(command=="{")
        {
            m_count++;
            return;
        }
        auto list=get_executor().lock()->getCommandList();
        list->push_back(command);
    };
    ~Dynamic_Commamd_Queue_Nested_Block_State()=default;
};

#endif // STATES_H
