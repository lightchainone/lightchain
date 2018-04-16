#ifndef __LCRPC_BUFFER_H
#define __LCRPC_BUFFER_H

namespace lcrpc
{
	
	class Buffer
	{
		char * _memory;		  
		unsigned int _capacity;		  
		unsigned int _size;		  
	plclic:
		
		Buffer() : _memory(0), _capacity(0), _size(0)
		{
		}
		
		void resize(unsigned int newsize)
		{
			_size = newsize;
			if(newsize > _capacity)
			{
				char * newmemory = new char[newsize + _capacity];
				memcpy(newmemory, _memory, _capacity);
				delete [] _memory;
				_memory = newmemory;
				_capacity += newsize;
			}
		}
		
		unsigned int size() const
		{
			return _size;
		}
		
		char * start()
		{
			return _memory;
		}
		
		~Buffer()
		{
			delete [] _memory;
		}
	};
}

#endif
