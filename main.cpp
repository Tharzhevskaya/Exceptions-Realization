#include <iostream>
#include <deque>

#include <setjmp.h>
#include <functional>

using namespace std;

enum exceptions { exc1 = 1, exc2 = 2, exc3 = 3, exc4 = 4 };

#define CATCH(x) }\
			     else if (setjmp_return_code == x)\
			     {

#define TRY setjmp_return_code = setjmp(point); \
            if (setjmp_return_code == 0) \
            {

#define MARK_VARIABLE_TO_DELETE(x) PushInDestructorsArray(x);

#define ETRY } else { \
                cout << "Undefined exception occured with code " << setjmp_return_code << endl; \
                }\
               destructors_array.clear();

// если во время очисткти стека произошло исключение        
#define THROW(x) MyThrow(x);


deque <function <void()> > destructors_array;
jmp_buf point;
int setjmp_return_code = 0;


bool clean_process = false;

// добавляем в список деструкторов деструктор объекста obj
template <typename T>
    void PushInDestructorsArray(T& obj) {
        destructors_array.push_front([&]() {
            obj.~T();
        });
    }

// чистим стек между вызовами setjmp и longjmp 
void MyThrow(int x){
	if (clean_process) {
        cout << "Exception occured while stack was cleaning! :(" << endl;
        exit(1); 
    }
    clean_process = true;
  	for (int i = 0; i < destructors_array.size(); i++) {
    	destructors_array[i]();
  	}
  	destructors_array.clear();
    clean_process = false;
  	longjmp(point, x);
}

struct TestCases {
  TestCases(int a) : test_cases(a) {
    cout << "Create new TestCase: " << test_cases << endl;
  }
  ~TestCases() {
    cout << "Delete TestCase: " << test_cases << endl;
  }
  int test_cases = 0;
};

void test2(int a){
  TestCases test_temporary = TestCases(a);
}

void test3(int a){
  TestCases test_temporary = TestCases(a);
  MARK_VARIABLE_TO_DELETE(test_temporary)
  THROW(exc3);
}

struct TestCasesWithExceptionInDestructor {
  TestCasesWithExceptionInDestructor(int a) : test_cases(a) {
    cout << "Create new TestCase: " << test_cases << endl;
  }
  ~TestCasesWithExceptionInDestructor() {
    cout << "Delete TestCase With Exception: " << test_cases << endl;
    THROW(4);
  }
  int test_cases = 0;
};

void tests(){
  TRY
    {
      cout << "TestCase #1" << endl;
      TestCases test_temporary = TestCases(1);

      MARK_VARIABLE_TO_DELETE(test_temporary)
      THROW( exc3 );
    }
  CATCH(exc1)
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH( exc2 )
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH( exc3 )
    {
      cout << "exc3 occured :(" << endl;
    }
  CATCH( exc4 )
    {
      cout << "exc4 occured :(" << endl;
    }
  ETRY

  TRY
    {
      cout << "TestCase #2" << endl;
      test2(2);
      THROW( exc2 );
    }
  CATCH(exc1)
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH( exc2 )
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH( exc3 )
    {
      cout << "exc3 occured :(" << endl;
    }
  CATCH( exc4 )
    {
      cout << "exc4 occured :(" << endl;
    }
  ETRY

  TRY
    {
      cout << "TestCase #3" << endl;
      test3(3);
    }
  CATCH(exc1)
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH( exc2 )
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH( exc3 )
    {
      cout << "exc3 occured :(" << endl;
    }
  CATCH( exc4 )
    {
      cout << "exc4 occured :(" << endl;
    }
  ETRY

  TRY
    {
      cout << "TestCase #4: Unexpected code of ecxeption" << endl;
      THROW(8);
    }
  CATCH(exc1)
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH(exc2)
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH( exc3 )
    {
      cout << "exc3 occured :(" << endl;
    }
  CATCH( exc4 )
    {
      cout << "exc4 occured :(" << endl;
    }
  ETRY

  TRY
    {
      cout << "TestCase #5: Exception occuredwhile stack was cleaning! :((" << endl;
      TestCasesWithExceptionInDestructor test_temporary = TestCasesWithExceptionInDestructor(5);
      MARK_VARIABLE_TO_DELETE(test_temporary)
      THROW( exc3 );
    }
  CATCH(exc1)
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH(exc2)
    {
      cout << "exc1 occured :(" << endl;
    }
  CATCH( exc3 )
    {
      cout << "exc3 occured :(" << endl;
    }
  CATCH( exc4 )
    {
      cout << "exc4 occured :(" << endl;
    }
  ETRY
}


int main()
{
  tests();

  return 0;
}
