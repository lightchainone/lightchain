


#ifndef  __LCSHAREPTR_H_
#define  __LCSHAREPTR_H_
#if 0
#include "Lsc/pool.h"
#include "Lsc/exception/Lsc_exception.h"
#include "lcatomic.h"

namespace lc
{
	template <typename T>
	class SmartRef
	{
		plclic:
			typedef T Type;
			typedef Lsc::mempool Pool;

		plclic:
			class Unit
			{
				typedef Lsc::mempool Pool;
				AtomicInt _first;
				Type _second;
				Pool *_pool;
			plclic:
				Unit() : _first(1), _pool(0) {}
				template <typename T_arg1>
				Unit(T_arg1 arg1) : _first(1), _second (arg1), _pool(0) {}
				template <typename T_arg1, typename T_arg2>
				Unit(T_arg1 arg1, T_arg2 arg2) : _first(1), _second (arg1, arg2), _pool(0) {}
				template <typename T_arg1, typename T_arg2, typename T_arg3>
				Unit(T_arg1 arg1, T_arg2 arg2, T_arg3 arg3) : _first(1), _second (arg1, arg2, arg3), _pool(0) {}
				template <typename T_arg1, typename T_arg2, typename T_arg3, typename T_arg4>
				Unit(T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4) : _first(1), _second (arg1, arg2, arg3, arg4), _pool(0) {}
				template <typename T_arg1, typename T_arg2, typename T_arg3, typename T_arg4, typename T_arg5>
				Unit(T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4, T_arg5 arg5) : _first(1), _second (arg1, arg2, arg3, arg4, arg5), _pool(0) {}

				inline void savePool(Pool *p) {
					_pool = p;
				}
				inline Pool * pool() { return _pool; }

				inline Unit & addRef()
				{
					_first.getAndIncrement();
					return *this;
				}

				inline bool release()
				{
					if (_first.decrementAndGet() == 0) {
						return true;
					}
					return false;
				}
			plclic:
				friend class SmartRef;
			};
		private:
			inline Unit * alloc(Pool *p) {
				if (p == NULL) {
					_unit = static_cast<Unit *>(::malloc(sizeof(Unit)));
				} else {
					_unit = static_cast<Unit *>(p->malloc(sizeof(Unit)));
				}
				if (_unit == NULL) {
					throw Lsc::Exception()<<BSL_EARG<<"not enough memory for malloc SmartRef";
				}
				_unit->savePool(p);
				return _unit;
			}
			inline void free() {
				Pool *p = _unit->pool();
				_unit->~Unit();
				if (p == NULL) {
					::free (_unit);
				} else {
					p->free(_unit, sizeof(Unit));
				}
				_unit = 0;
			}

		plclic:
			SmartRef() 
			{
				::new(this->alloc(NULL)) Unit;
			}
			SmartRef(Pool *p) {
				::new(this->alloc(p)) Unit;
			}
			SmartRef(const SmartRef &__ref) 
			{
				_unit = & __ref._unit->addRef();
			}
			template <typename T_arg1>
			SmartRef(T_arg1 arg1)
			{
				::new(this->alloc(NULL)) Unit(arg1);
			}
			template <typename T_arg1>
			SmartRef(Pool *p, T_arg1 arg1)
			{
				::new(this->alloc(p)) Unit(arg1);
			}
			template <typename T_arg1, typename T_arg2>
			SmartRef(T_arg1 arg1, T_arg2 arg2)
			{
				::new(this->alloc(NULL)) Unit(arg1, arg2);
			}
			template <typename T_arg1, typename T_arg2>
			SmartRef(Pool *p, T_arg1 arg1, T_arg2 arg2)
			{
				::new(this->alloc(p)) Unit(arg1, arg2);
			}
			template <typename T_arg1, typename T_arg2, typename T_arg3>
			SmartRef(T_arg1 arg1, T_arg2 arg2, T_arg3 arg3)
			{
				::new(this->alloc(NULL)) Unit(arg1, arg2, arg3);
			}
			template <typename T_arg1, typename T_arg2, typename T_arg3>
			SmartRef(Pool *p, T_arg1 arg1, T_arg2 arg2, T_arg3 arg3)
			{
				::new(this->alloc(p)) Unit(arg1, arg2, arg3);
			}
			template <typename T_arg1, typename T_arg2, typename T_arg3, typename T_arg4>
			SmartRef(T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4)
			{
				::new(this->alloc(NULL)) Unit(arg1, arg2, arg3, arg4);
			}
			template <typename T_arg1, typename T_arg2, typename T_arg3, typename T_arg4>
			SmartRef(Pool *p, T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4)
			{
				::new(this->alloc(p)) Unit(arg1, arg2, arg3, arg4);
			}
			template <typename T_arg1, typename T_arg2, typename T_arg3, typename T_arg4, typename T_arg5>
			SmartRef(T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4, T_arg5 arg5)
			{
				::new(this->alloc(NULL)) Unit(arg1, arg2, arg3, arg4, arg5);
			}
			template <typename T_arg1, typename T_arg2, typename T_arg3, typename T_arg4, typename T_arg5>
			SmartRef(Pool *p, T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4, T_arg5 arg5)
			{
				::new(this->alloc(p)) Unit(arg1, arg2, arg3, arg4, arg5);
			}
			~SmartRef()
			{
				this->release();
			}
			inline SmartRef & operator = (const SmartRef &__ref)
			{
				if (_unit == __ref._unit) {
					return *this;
				}
				this->release();
				_unit = & ref._unit->addRef();
				return *this;
			}

			Type & operator * () {
				return _unit->_second;
			}

			Type * operator -> () {
				return & _unit->_second;
			}

			int ref() const {
				return _unit->_first.get();
			}
			Unit & addRef()
			{
				return _unit->addRef();
			}
			void release() {
				if (_unit->release()) {
					this->free();
				}
			}

		private:
			Unit *_unit;
	};
};
#endif
#endif  


