#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>


const int TEST_SOCKET = 999;
const int TEST_RESULT = 999;
const int TEST_VALUE = 777;
const char TEST_STRING[] = { "test_string" };


//класс ответа
class QueryResult {
    public:
        int value = TEST_VALUE;
        bool operator==(const QueryResult qResult) const {
            return this->value == qResult.value;
        }
};


class DBConnectionInterface {
    public:
        DBConnectionInterface() {}
        virtual ~DBConnectionInterface() {}
        virtual int open(int socket) = 0;
        virtual bool close(int socket) = 0;
        virtual void execQuery(QueryResult& qResult, const char* query) = 0;
};


class ClassThatUseDb : public DBConnectionInterface {
    public:
        ClassThatUseDb() {}
        virtual ~ClassThatUseDb() {}
        int open(int socket) { return TEST_RESULT; }
        bool close(int socket) { return true;}
        void execQuery(QueryResult& qResult, const char* query) {
            qResult.value = TEST_VALUE;
        }
};


class MockDBConnection : public DBConnectionInterface {
    public:
        MOCK_METHOD(int, open, (int socket), (override));
        MOCK_METHOD(bool, close, (int socket), (override));
        MOCK_METHOD(void, execQuery, (QueryResult& qResult, const char* query), (override));
};


//создаем фикстуру и тирдаун для наших тестов «на всякий случай»
class SomeTestSuite : public ::testing::Test {
    protected:
        void SetUp() {
            someDBConnection = new ClassThatUseDb();
        }

        void TearDown() {
            delete someDBConnection;
        }

    protected:
        DBConnectionInterface* someDBConnection;
};


TEST_F(SomeTestSuite, testcase1) {
    int socket = TEST_SOCKET;
    int iResult = someDBConnection->open(socket);
    int reference = TEST_RESULT;
    ASSERT_EQ(iResult, reference);
}


TEST_F(SomeTestSuite, testcase2) {
    int socket = TEST_SOCKET;
    bool bResult = someDBConnection->close(socket);
    bool reference = true;
    ASSERT_EQ(bResult, reference);
}


//тест на обмен с использованием мок-объектов
TEST_F(SomeTestSuite, testcase3) {
    //данные, которые мы планируем использовать как данные и ответы мок-объекта
    QueryResult qResult;

    //создаем мок-объект
    MockDBConnection mockDBcon;

    EXPECT_CALL(mockDBcon, open(TEST_SOCKET)).WillOnce(::testing::Return(TEST_VALUE));
    EXPECT_CALL(mockDBcon, close(TEST_SOCKET)).WillOnce(::testing::Return(true));

    ////запускаем алгоритм на обработкуl
    mockDBcon.execQuery(qResult, TEST_STRING);

    //сравниваем полученный результат с референсом
    ASSERT_EQ(qResult.value, TEST_VALUE);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

