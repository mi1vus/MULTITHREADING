#include <list>
#include <iostream>
#include <cassert>
#define buf 400/*1048576*/
/*
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#define timesub(a,b,res) \
do{
\
	(res)->tv_sec = (a)->tv_sec - (b)->tv_sec; \
	(res)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
if ((res)->tv_usec < 0){
	\
	--(res)->tv_sec; \
	(res)->tv_usec += 1000000; \
} \
} while (0);
*/

class MBlock{
public:
	MBlock(void* addr, unsigned int size) :size(size), address(addr){};
	int size;
	void* address;
};

class SmallAllocator {
private:
	char mem[buf];
	const unsigned int msize;
	std::list<MBlock*>* blocks;
	std::list<MBlock*>* allocated;
public:
	SmallAllocator() :msize(buf)
	{
		blocks = new std::list<MBlock*>();
		auto newBlock = new MBlock(mem, buf);
		blocks->push_front(newBlock);
		allocated = new std::list<MBlock*>();
	}

	void *Alloc(unsigned int allocSize) {
		if (!mem || !allocSize || !blocks || blocks->size() == 0)
			return nullptr;

		//���� ������ ���������� ����
		auto cuttingBlock = blocks->begin();
		MBlock* newBlock = nullptr;
		for (cuttingBlock; cuttingBlock != blocks->end(); ++cuttingBlock)
		{
			//���� ���� �������� �� ���������� ��� � ��������� ���������� ������
			if ((*cuttingBlock)->size >= allocSize)
			{
				unsigned int newSize = (*cuttingBlock)->size - allocSize;
				void* newAddr = ((char*)(*cuttingBlock)->address + allocSize);
				if (newSize > 0)
					newBlock = new MBlock((newAddr), newSize);
				break;
			}
		}

		//���� ����� ����������� �������
		if (cuttingBlock != blocks->end())
		{
			if (newBlock != nullptr)
			{
				//��������� ���������� ������ � ����� ����
				blocks->insert(cuttingBlock, newBlock);
			}
			//������� ������ ����
			void* cuttingBlockAddress = (*cuttingBlock)->address;
			blocks->remove(*cuttingBlock);

			//��������� ���������� ����
			allocated->push_back(new MBlock(cuttingBlockAddress, allocSize));

			//���������� �����
			return cuttingBlockAddress;
		}

		return nullptr; /* out of memory */
	}

	void Free(void *Pointer)
	{
		if (!mem || !Pointer || !blocks || allocated->size() == 0)
			return;

		if (Pointer < mem || Pointer >= mem + msize)
			return;

		//���� � ��� ���������� ������ ������ ����
		auto remBlock = allocated->begin();
		MBlock* newBlock = nullptr;
		for (remBlock; remBlock != allocated->end(); ++remBlock)
		{
			//����� ������� �����
			if ((*remBlock)->address == Pointer)
			{
				unsigned int newSize = (*remBlock)->size;
				void* newAddr = (*remBlock)->address;

				//���� ����������� �����
				auto cuttingBlock = blocks->begin();
				MBlock* remBlock1 = nullptr;
				MBlock* remBlock2 = nullptr;
				for (cuttingBlock; cuttingBlock != blocks->end(); ++cuttingBlock)
				{					
					//������ �� ���������� ���� ��������� ����� - ��������� �����
					if ((*cuttingBlock)->address == (char*)newAddr + newSize)
					{
						newSize += (*cuttingBlock)->size;
						remBlock1 = *cuttingBlock;
					}
					//����� �� ���������� ���� ��������� ����� - ��������� �����
					else if (newAddr == (char*)(*cuttingBlock)->address + (*cuttingBlock)->size)
					{
						newAddr = (*cuttingBlock)->address;
						newSize += (*cuttingBlock)->size;
						remBlock2 = *cuttingBlock;
					}
				}
				
				if (remBlock1)
					blocks->remove(remBlock1);
				if (remBlock2)
					blocks->remove(remBlock2);

				//��������� ������������ ����� ����
				if (newSize > 0)
					newBlock = new MBlock(newAddr, newSize);
				break;
			}
		}

		//���� ����� ��������� - ������ ��� �� ������� ������
		if (remBlock != allocated->end())
		{
			allocated->remove(*remBlock);
		}


		//��������� ������������ ���� � ������ �����������
		if (newBlock)
		{
			blocks->push_front(newBlock);
		} 
	}
	
	void *ReAlloc(void *Pointer, unsigned int reallocSize)
	{
		if (!Pointer)
		{
			return Alloc(reallocSize);
		}
		if (!reallocSize && Pointer)
		{
			Free(Pointer);
			return nullptr;
		}

		//���� ���� ����
		auto reallBlock = allocated->begin();
		for (reallBlock; reallBlock != allocated->end(); ++reallBlock)
		{
			if ((*reallBlock)->address == Pointer)
			{
				break;
			}
		}
		//���� �� ����� ������ ��������
		if (reallBlock == allocated->end())
		{
//			return Alloc(reallocSize);
		}

		void* result = nullptr;
		MBlock* newBlock = nullptr;
		//���� ����� �������� ������� ������ - ���������� ������ ������ � ���������� ������� ������ � ���������
		if ((*reallBlock)->size >= reallocSize)
		{
			result = (*reallBlock)->address;
			if ((*reallBlock)->size > reallocSize)
			{
				unsigned int newSize = (*reallBlock)->size - reallocSize;
				void* newAddr = (char*)(*reallBlock)->address + reallocSize;
				//��������� ��������� ����
				(*reallBlock)->size = reallocSize;
				//������� ����� ���� ���������� ������
				newBlock = new MBlock(newAddr, newSize);
			}
		}
		//���� ����� ������ ������ - ����� �� �������� � �������� ������ �� ������ � �����
		else
		{
			result = Alloc(reallocSize);
			char* it = (char*)(*reallBlock)->address;
			char* end = (char*)(*reallBlock)->address + (*reallBlock)->size;
			char* dst = (char*)result;
			for (it; it != end; ++it)
			{
				*dst = *it;
				++dst;
			}
			//������� ������ ���� ������
			Free((*reallBlock)->address);

			return result;
		}

		//�������� ��������� ������ ���� ��� ����
		if (newBlock)
		{
			blocks->push_front(newBlock);
		}
		return result;
	}
};

int main(int argc, char **argv){

		SmallAllocator A2;

		int * all = (int *)A2.Alloc(buf);
		for (unsigned int i = 0; i < buf / 4; i++) all[i] = 2 * i + 5;
		for (unsigned int i = 0; i < buf / 4; i++) assert(all[i] == 2 * i + 5);
		A2.Free(all);

		int * in1,* in2,* in3,* in4,* in5,* in6,* in7,* in8,* in9;
			
		in1 = (int *)A2.Alloc(1 * sizeof(int));
		*in1 = 11;
		in2 = (int *)A2.Alloc(1 * sizeof(int));
		*in2 = 22;

		A2.Free(in2);

		in3 = (int *)A2.Alloc(1 * sizeof(int));
		*in3 = 33;
		in4 = (int *)A2.Alloc(1 * sizeof(int));
		*in4 = 44;
		in5 = (int *)A2.Alloc(1 * sizeof(int));
		*in5 = 55;
		in6 = (int *)A2.Alloc(1 * sizeof(int));
		*in6 = 66;

		assert(*in1 == 11);
		assert(*in3 == 33);
		assert(*in4 == 44);
		assert(*in5 == 55);
		assert(*in6 == 66);

		in2 = (int *)A2.Alloc(1 * sizeof(int));
		*in2 = 222;
		assert(*in2 == 222);

		A2.Free(in3);
		A2.Free(in5);
		A2.Free(in4);

		in6 = (int *)A2.ReAlloc(in6, 2 * sizeof(int));
		in6[1] = 67;

		assert(*in1 == 11);
		assert(*in2 == 222);
		assert(in6[0] == 66);
		assert(in6[1] == 67);
		
		in2 = (int *)A2.ReAlloc(in2, 3 * sizeof(int));

		in2[1] = 223;
		in2[2] = 224;

		assert(in2[0] == 222);
		assert(in2[1] == 223);
		assert(in2[2] == 224);
		assert(*in1 == 11);
		assert(in6[0] == 66);
		assert(in6[1] == 67);

		A2.Free(in1);
		A2.Free(in6);

		assert(in2[0] == 222);
		assert(in2[1] == 223);
		assert(in2[2] == 224);

		A2.Free(in2);

		all = (int *)A2.Alloc(buf);
		for (unsigned int i = 0; i < buf/4 + 1; i++) all[i] = 2*i+5;
		for (unsigned int i = 0; i < buf / 4 + 1; i++) assert( all[i] == 2 * i + 5);
		A2.Free(all);

		int * A2_P3 = (int *)A2.Alloc(10 * sizeof(int));
		for (unsigned int i = 0; i < 10; i++) A2_P3[i] = i;
		for (unsigned int i = 0; i < 10; i++) assert(A2_P3[i] == i);

		int * A2_P4 = (int *)A2.Alloc(10 * sizeof(int));
		for (unsigned int i = 0; i < 10; i++) A2_P4[i] = i;
		A2_P4 = (int *)A2.ReAlloc(A2_P4, 20 * sizeof(int));
		for (unsigned int i = 10; i < 20; i++) A2_P4[i] = i;
		for (unsigned int i = 0; i < 10; i++) assert(A2_P3[i] == i);
		for (unsigned int i = 0; i < 20; i++) assert(A2_P4[i] == i);

		A2.Free(A2_P3);
		A2.Free(A2_P4);

		all = (int *)A2.Alloc(buf);
		for (unsigned int i = 0; i < buf / 4 + 1; i++) all[i] = 2 * i + 5;
		for (unsigned int i = 0; i < buf / 4 + 1; i++) assert(all[i] == 2 * i + 5);
		A2.Free(all);

		int * A2_P1 = (int *)A2.Alloc(20 * sizeof(int));
		for (unsigned int i = 0; i < 20; i++) A2_P1[i] = i;
		A2_P1 = (int *)A2.ReAlloc(A2_P1,10 * sizeof(int));
		for (unsigned int i = 0; i < 10; i++) assert(A2_P1[i] == i);

		int * A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
		for (unsigned int i = 0; i < 10; i++) A2_P2[i] = i;
		A2_P2 = (int *)A2.ReAlloc(A2_P2, 20 * sizeof(int));
		for (unsigned int i = 10; i < 20; i++) A2_P2[i] = i;
		for (unsigned int i = 0; i < 10; i++) assert(A2_P1[i] == i);
		for (unsigned int i = 0; i < 20; i++) assert(A2_P2[i] == i);

	return 0;
}