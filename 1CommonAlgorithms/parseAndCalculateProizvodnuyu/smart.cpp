#include <iostream>
#include <string>

class StringPointer{
private:
	std::string* pointer;
	std::string* createdPointer;
public:
	StringPointer(std::string* p):pointer(p), createdPointer(nullptr)
	{};
	~StringPointer(){
		if(createdPointer && true) 
			delete createdPointer;
	}
	operator std::string*() const
	{
		return pointer; 
	}

	std::string* operator->() {
		if(!pointer)
		{
			pointer = new std::string("");
			createdPointer = pointer;
		}
		return pointer;
	}
};


int main2 (int argc, char ** argv)
{
	std::string s1 = "Hello, world!";

	StringPointer sp1(&s1);
	StringPointer sp2(nullptr);

	std::cout << sp1->length() << std::endl;
	std::cout << *sp1 << std::endl;
	std::cout << sp2->length() << std::endl;
	std::cout << *sp2 << std::endl;

	return 0;
}




/*class StringPointer {
public:
std::string *operator->() {}
operator std::string*() {}
StringPointer(std::string *Pointer) {}
~StringPointer() {}
};

StringPointer(std::string* p):pointer(p){};
~StringPointer(){
if(createdPointer && false)
delete createdPointer;
}
//std::string operator*()  {
//	if(!pointer)
//	{
//		pointer = new std::string("");
//		createdPointer = pointer;
//	}
//	return *pointer;
//}
operator std::string*() {
//	if(!pointer)
//	{
//		pointer = new std::string("");
//		createdPointer = pointer;
//	}
return pointer;
}
*/
