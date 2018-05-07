



#ifndef __SORT_IDL_H_
#define __SORT_IDL_H_
#include "mcpack_idm.h"
#include "compack/compack.h"
namespace idm
{
	class qsort_sort_params;
	class qsort_sort_result_params;
	class qsort_sort_response;
	class qsort_echo_params;
	class qsort_echo_result_params;
	class qsort_echo_response;
	class qsort_sort_params : plclic idm::CompackBean
	{
	plclic:
		
		static qsort_sort_params * create(Lsc::mempool *pool);
		
		static void destroy(qsort_sort_params * data);
		
		explicit qsort_sort_params( Lsc::mempool *pool );
		
		qsort_sort_params(const qsort_sort_params& _value);
		
		qsort_sort_params & operator =(const qsort_sort_params& _value);
		
		virtual ~qsort_sort_params();
		
		virtual void load(const void *buffer, unsigned int size);
		
		virtual void save(void *buffer, unsigned int size);
		virtual void save(compack::buffer::Object *object);
		
		void clear();
		
		void detach();
		
		
		const idm::vector<uint32_t> & indata() const;
		
		idm::vector<uint32_t> * m_indata();
		
		unsigned int indata_size() const;
		
		uint32_t indata(unsigned int i0) const;
		
		void set_indata(unsigned int i0, uint32_t value);
		
		bool has_indata() const;
		
		void clear_indata();
		
		idm::Unknown & unknown();
		
		const idm::Unknown & unknown() const;
		
		unsigned int unknown_size() const;
	protected:
		idm::vector<uint32_t> _m_indata;
		mutable idm::Unknown * _unknown;
		void assign (const qsort_sort_params& _value);
	};
	class qsort_sort_result_params : plclic idm::CompackBean
	{
	plclic:
		
		static qsort_sort_result_params * create(Lsc::mempool *pool);
		
		static void destroy(qsort_sort_result_params * data);
		
		explicit qsort_sort_result_params( Lsc::mempool *pool );
		
		qsort_sort_result_params(const qsort_sort_result_params& _value);
		
		qsort_sort_result_params & operator =(const qsort_sort_result_params& _value);
		
		virtual ~qsort_sort_result_params();
		
		virtual void load(const void *buffer, unsigned int size);
		
		virtual void save(void *buffer, unsigned int size);
		virtual void save(compack::buffer::Object *object);
		
		void clear();
		
		void detach();
		
		
		const idm::vector<uint32_t> & outdata() const;
		
		idm::vector<uint32_t> * m_outdata();
		
		unsigned int outdata_size() const;
		
		uint32_t outdata(unsigned int i0) const;
		
		void set_outdata(unsigned int i0, uint32_t value);
		
		bool has_outdata() const;
		
		void clear_outdata();
		
		idm::Unknown & unknown();
		
		const idm::Unknown & unknown() const;
		
		unsigned int unknown_size() const;
	protected:
		idm::vector<uint32_t> _m_outdata;
		mutable idm::Unknown * _unknown;
		void assign (const qsort_sort_result_params& _value);
	};
	class qsort_sort_response : plclic idm::CompackBean
	{
	plclic:
		
		static qsort_sort_response * create(Lsc::mempool *pool);
		
		static void destroy(qsort_sort_response * data);
		
		explicit qsort_sort_response( Lsc::mempool *pool );
		
		qsort_sort_response(const qsort_sort_response& _value);
		
		qsort_sort_response & operator =(const qsort_sort_response& _value);
		
		virtual ~qsort_sort_response();
		
		virtual void load(const void *buffer, unsigned int size);
		
		virtual void save(void *buffer, unsigned int size);
		virtual void save(compack::buffer::Object *object);
		
		void clear();
		
		void detach();
		
		
		const qsort_sort_result_params & result_params() const;
		
		qsort_sort_result_params * m_result_params();
		
		bool has_result_params() const;
		
		void clear_result_params();
		
		idm::Unknown & unknown();
		
		const idm::Unknown & unknown() const;
		
		unsigned int unknown_size() const;
	protected:
		qsort_sort_result_params * _m_result_params;
		mutable idm::Unknown * _unknown;
		void assign (const qsort_sort_response& _value);
	};
	class qsort_echo_params : plclic idm::CompackBean
	{
	plclic:
		
		static qsort_echo_params * create(Lsc::mempool *pool);
		
		static void destroy(qsort_echo_params * data);
		
		explicit qsort_echo_params( Lsc::mempool *pool );
		
		qsort_echo_params(const qsort_echo_params& _value);
		
		qsort_echo_params & operator =(const qsort_echo_params& _value);
		
		virtual ~qsort_echo_params();
		
		virtual void load(const void *buffer, unsigned int size);
		
		virtual void save(void *buffer, unsigned int size);
		virtual void save(compack::buffer::Object *object);
		
		void clear();
		
		void detach();
		
		
		const char * instr(unsigned int * length = 0) const;
		
		void set_instr(const char * str, int length = -1);
		
		void set_ptr_instr(const char * str, int length = -1);
		
		bool has_instr() const;
		
		void clear_instr();
		
		idm::Unknown & unknown();
		
		const idm::Unknown & unknown() const;
		
		unsigned int unknown_size() const;
	protected:
		idm::string _m_instr;
		mutable idm::Unknown * _unknown;
		void assign (const qsort_echo_params& _value);
	};
	class qsort_echo_result_params : plclic idm::CompackBean
	{
	plclic:
		
		static qsort_echo_result_params * create(Lsc::mempool *pool);
		
		static void destroy(qsort_echo_result_params * data);
		
		explicit qsort_echo_result_params( Lsc::mempool *pool );
		
		qsort_echo_result_params(const qsort_echo_result_params& _value);
		
		qsort_echo_result_params & operator =(const qsort_echo_result_params& _value);
		
		virtual ~qsort_echo_result_params();
		
		virtual void load(const void *buffer, unsigned int size);
		
		virtual void save(void *buffer, unsigned int size);
		virtual void save(compack::buffer::Object *object);
		
		void clear();
		
		void detach();
		
		
		const char * outstr(unsigned int * length = 0) const;
		
		void set_outstr(const char * str, int length = -1);
		
		void set_ptr_outstr(const char * str, int length = -1);
		
		bool has_outstr() const;
		
		void clear_outstr();
		
		idm::Unknown & unknown();
		
		const idm::Unknown & unknown() const;
		
		unsigned int unknown_size() const;
	protected:
		idm::string _m_outstr;
		mutable idm::Unknown * _unknown;
		void assign (const qsort_echo_result_params& _value);
	};
	class qsort_echo_response : plclic idm::CompackBean
	{
	plclic:
		
		static qsort_echo_response * create(Lsc::mempool *pool);
		
		static void destroy(qsort_echo_response * data);
		
		explicit qsort_echo_response( Lsc::mempool *pool );
		
		qsort_echo_response(const qsort_echo_response& _value);
		
		qsort_echo_response & operator =(const qsort_echo_response& _value);
		
		virtual ~qsort_echo_response();
		
		virtual void load(const void *buffer, unsigned int size);
		
		virtual void save(void *buffer, unsigned int size);
		virtual void save(compack::buffer::Object *object);
		
		void clear();
		
		void detach();
		
		
		const qsort_echo_result_params & result_params() const;
		
		qsort_echo_result_params * m_result_params();
		
		bool has_result_params() const;
		
		void clear_result_params();
		
		idm::Unknown & unknown();
		
		const idm::Unknown & unknown() const;
		
		unsigned int unknown_size() const;
	protected:
		qsort_echo_result_params * _m_result_params;
		mutable idm::Unknown * _unknown;
		void assign (const qsort_echo_response& _value);
	};
}
#endif 
