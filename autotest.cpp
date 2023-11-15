#define BOOST_TEST_MODULE autotest
#include "states.h"


#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
using boost::test_tools::output_test_stream;

#include <string>
#include <list>

output_test_stream output;

//Устанавливаем указатель на выходной поток
struct GlobalFixture {

    GlobalFixture() {
      stream_ptr=&(output);
  }

  ~GlobalFixture() {
      stream_ptr=&(std::cout);
  }

};

BOOST_TEST_GLOBAL_FIXTURE( GlobalFixture );



//Создание и инициализация исполнителя команд
std::shared_ptr<Executor> init(int n)
{
    SharedExecutorData data;
    data.size=n;
    auto executor=std::shared_ptr<Executor>(new Executor(data));
    std::shared_ptr<StateFactory> factory(new StateFactory);

    #define REGISTRATE(X) factory->registerate<State_Creator<X,Executor_ptr>, Executor_ptr> (X::id, Executor_ptr(executor));

    REGISTRATE(Simple_Commamd_Queue_State)
    REGISTRATE(Dynamic_Commamd_Queue_State)
    REGISTRATE(Dynamic_Commamd_Queue_Nested_Block_State)

    executor->setFactory(factory);
    return executor;
}


BOOST_AUTO_TEST_SUITE(autotest)

//Тест начального состояния при n=4
BOOST_AUTO_TEST_CASE(test_1_4)
{
    std::list<std::string> list {"cmd1", "cmd2", "cmd3", "cmd4", "cmd5"};
    auto executor=init(4);
    for(auto x: list) executor->execute(x);
    BOOST_CHECK( output.is_equal( "bulk: cmd1, cmd2, cmd3, cmd4\n" ) );
    output.clear();

    executor->execute("EOF");
    BOOST_CHECK( output.is_equal( "bulk: cmd5\n" ) );
    output.clear();

}


//Тест начального состояния при n=3
BOOST_AUTO_TEST_CASE(test_1_3)
{
    std::list<std::string> list {"cmd1", "cmd2", "cmd3", "cmd4", "cmd5"};
    auto executor=init(3);
    for(auto x: list) executor->execute(x);
    BOOST_CHECK( output.is_equal( "bulk: cmd1, cmd2, cmd3\n" ) );
    output.clear();

    executor->execute("EOF");
    BOOST_CHECK( output.is_equal( "bulk: cmd4, cmd5\n" ) );
    output.clear();

}



//Тест перехода во второе состояние и выхода из него
BOOST_AUTO_TEST_CASE(test_2)
{
    std::list<std::string> list {"cmd4", "cmd5", "{"};
    auto executor=init(3);
    for(auto x: list) executor->execute(x);
    BOOST_CHECK( output.is_equal( "bulk: cmd4, cmd5\n" ) );
    output.clear();

    std::list<std::string> list2 {"cmd6", "cmd7", "}"};
    for(auto x: list2) executor->execute(x);
    BOOST_CHECK( output.is_equal( "bulk: cmd6, cmd7\n" ) );
    output.clear();

    std::list<std::string> list3 {"{", "cmd6", "cmd7", "EOF"};
    for(auto x: list3) executor->execute(x);
    BOOST_CHECK( output.is_equal( "\n" ) );

}

//Тест перехода в третье состояние и выхода из него
BOOST_AUTO_TEST_CASE(test_3)
{
    std::list<std::string> list1 {"{", "cmd6", "cmd7", "{", "cmd8", "cmd9", "{","cmd10", "cmd11", "}"};
    auto executor=init(3);
    for(auto x: list1) executor->execute(x);
    BOOST_CHECK( output.is_equal( "\n" ) );
    output.clear();

    std::list<std::string> list2 {"cmd12", "}", "}"};
    for(auto x: list2) executor->execute(x);
    BOOST_CHECK( output.is_equal( "bulk: cmd6, cmd7, cmd8, cmd9, cmd10, cmd11, cmd12\n" ) );
    output.clear();

}



BOOST_AUTO_TEST_SUITE_END()
