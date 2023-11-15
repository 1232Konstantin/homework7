#include <iostream>
#include "states.h"
#include <fstream>

using namespace std;

bool TEST=false;
std::fstream stream;


int main(int argc, char *argv[])
{
   int n=(argc==2)? std::stoi(std::string(argv[1])) : 3;
   SharedExecutorData data;
   data.size=n;
   auto executor=std::shared_ptr<Executor>(new Executor(data));
   std::shared_ptr<StateFactory> factory(new StateFactory);

   #define REGISTRATE(X) factory->registerate<State_Creator<X,Executor_ptr>, Executor_ptr> (X::id, Executor_ptr(executor));

   REGISTRATE(Simple_Commamd_Queue_State)
   REGISTRATE(Dynamic_Commamd_Queue_State)
   REGISTRATE(Dynamic_Commamd_Queue_Nested_Block_State)

   executor->setFactory(factory);

   if (TEST)
   {
       stream.open("D:/test3.txt");
       for(std::string line; std::getline(stream, line);)
       {
           if (!stream.eofbit) line="EOF";
           executor->execute(line);
       }
   }
   else
   {
       for(std::string line; std::getline(cin, line);)
       {
           if (!cin.eofbit) line="EOF";
           executor->execute(line);
       }
   }



    return 0;
}
