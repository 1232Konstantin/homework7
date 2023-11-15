#ifndef EXECUTER_H
#define EXECUTER_H
#include <string>
#include <memory>
#include <list>
#include <iostream>
#include "statefactory.h"

using CommandList_ptr=std::shared_ptr<std::list<std::string>>;

//Данные executor необходимые для кастомизации состояний
//Меняются в зависимости от протокола информационного обмена
struct SharedExecutorData
{
    size_t size; //в данном случае только размер блока
};


//Класс исполнитель команд
//остается неизменным при кастомизации состояний, но исходное состояние должно называться "FIRST_STATE"
class Executor
{
   std::unique_ptr<BaseState> m_state;
   std::shared_ptr<StateFactory> m_factory;

   CommandList_ptr m_command_list;
   SharedExecutorData m_data;

public:
   Executor(SharedExecutorData& data): m_data(data)
   {
       m_command_list=CommandList_ptr(new std::list<std::string>());
   }

   void setFactory(std::shared_ptr<StateFactory>& factory)
   {
       m_factory=std::move(factory);
       setState("FIRST_STATE");
   }

   void setState(std::string state)
   {
        m_state=m_factory->create(state);

   }
   CommandList_ptr getCommandList()
   {
       return m_command_list;
   }

   SharedExecutorData& getExecutorData() {return m_data;}

   void  execute(std::string command) {m_state->execute(command);}

   ~Executor()=default;
};

using Executor_ptr=std::weak_ptr<Executor>;





#endif // EXECUTER_H
