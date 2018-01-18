
#ifndef  __LC_ATOMIC_H_
#define  __LC_ATOMIC_H_

#include "comatomic.h"

namespace lc
{
	namespace 
	{
		template <typename T>
		struct AtomicTratis
		{
			typedef T type;
		};
		template <>
		struct AtomicTratis<int8_t>
		{
			typedef uint8_t type;
		};
		template <> 
		struct AtomicTratis<int16_t>
		{
			typedef uint16_t type;
		};
		template <>
		struct AtomicTratis<int32_t>
		{
			typedef uint32_t type;
		};
		template <>
		struct AtomicTratis<int64_t>
		{
			typedef uint64_t type;
		};
	}

	template <typename T>
	class Atomic
	{
		plclic:
			
			inline Atomic() : _value(0) {}

			
			inline Atomic(T initialValue) : _value(initialValue) {}

			
			inline T 	addAndGet(T delta) { return getAndAdd(delta) + delta; }
			
			inline bool 	compareAndSet(T expect, T update) 
			{ 
				return atomic_compare_exchange(
						&_value, update, expect) == typename AtomicTratis<T>::type(expect); 
			}

			
			inline T 	decrementAndGet() { return atomic_dec(&_value); }

			
			inline T 	incrementAndGet() { return atomic_inc(&_value); }

			
			inline T 	get() const { return _value; }

			
			inline void 	set(T newValue) { _value = newValue; }

			
			inline T 	getAndAdd(T delta) { return atomic_add (&_value, delta); }

			
			inline T 	getAndDecrement() { return decrementAndGet() + 1;}

			
			inline T 	getAndIncrement() { return incrementAndGet() - 1; }

			
			inline T 	getAndSet(T newValue) { return atomic_exchange (&_value, newValue); }

		private:
			typename AtomicTratis<T>::type  _value;
	};

	template <> class Atomic<bool> 
	{
		plclic:
			
			inline Atomic() : _atomic(0) {}

			
			inline Atomic(bool initialValue) : _atomic(initialValue?1:0) {}

			
			inline bool 	compareAndSet(bool expect, bool update)
			{
				return _atomic.compareAndSet(expect?1:0, update?1:0);
			}

			
			inline bool 	get()
			{
				return _atomic.get() != 0;
			}

			
			inline bool 	getAndSet(bool newValue)
			{
				return _atomic.getAndSet(newValue?1:0) != 0;
			}

			
			inline void 	set(bool newValue) 
			{
				_atomic.set(newValue?1:0);
			}
		private:
			Atomic<int8_t> _atomic;
	};

	template <typename T> class Atomic<T *>
	{
		plclic:
			inline Atomic() : _ptr(0) {}
			inline Atomic(const T *ptr) : _ptr((ptr_t)ptr) {}
			inline bool compareAndSet(const T *expect, const T *update)
			{
				return _ptr.compareAndSet((ptr_t)expect, (ptr_t)update);
			}
			inline T * get()
			{
				return (T *) _ptr.get();
			}
			inline void set(const T *newValue)
			{
				_ptr.set((ptr_t)newValue);
			}

		private:
#ifdef __x86_64__
			typedef uint64_t ptr_t;
#else
			typedef uint32_t ptr_t;
#endif
			Atomic<ptr_t> _ptr;
	};

	typedef Atomic<int32_t> AtomicInt;
	typedef Atomic<int64_t> AtomicLong;
	typedef Atomic<bool> AtomicBool;
};

#endif  


