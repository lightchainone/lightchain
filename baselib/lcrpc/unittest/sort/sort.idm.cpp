



#include "sort.idm.h"
namespace idm
{
	using namespace idm;
	using namespace compack;
	template<typename BUFFER>class qsort_sort_paramsCompack : plclic qsort_sort_params
	{
	plclic:
		
		void load(compack::Reader<BUFFER> & reader)
		try
		{
			clear();
			int __count = 0;
			ObjectIterator it;
			while (reader.next(it))
			{
				if (!strcmp(it.getName(), "indata"))
				{
					compack::Reader<BUFFER> reader0(reader, it);
					unsigned int size0 = reader0.count();
					if (size0 >0)
					{
						compack::Iterator it0;
						if (it.template isIsoArray<uint32_t>())
						{
							_m_indata.push_back(it.template getAsIsoArray<uint32_t>(), it.getIsoArrayLength());
						}
						else
						{
							_m_indata.resize(size0);
							for (unsigned int i0 = 0; i0 < (unsigned int)size0; i0++)
							{
								if (!reader0.next(it0))
								{
									throw Lsc::ParseErrorException() << BSL_EARG << "bad data" << "indata";
								}
								_m_indata.unsafe_get(i0) = it0.template getAsNum<uint32_t>();
							}
						}
					}
					__count ++;
					continue;
				}
				unknown().template push_back<ObjectIterator>(it);
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		catch (...)
		{
			clear();
			throw;
		}
		
		static void load(Lsc::mempool *, idm::vector<qsort_sort_params> & vec, compack::Reader<BUFFER> & arrayreader)
		{
			int __count = 0;
			ObjectIterator arrayit;
			compack::Reader<BUFFER> resizereader = arrayreader;
			if (resizereader.next(arrayit))
			{
				compack::Reader<BUFFER> resizereader1(resizereader, arrayit);
				vec.resize(resizereader1.count());
				while (arrayreader.next(arrayit))
				{
					if (!strcmp(arrayit.getName(), "indata"))
					{
						Iterator it;
						compack::Reader<BUFFER> reader(arrayreader, arrayit);
						for (idm::vector<qsort_sort_params>::iterator i = vec.begin(); i != vec.end(); ++i)
						{
							if (!reader.next(it))
							{
								throw Lsc::ParseErrorException() << BSL_EARG << "bad data";
							}
							compack::Reader<BUFFER> reader0(reader, it);
							unsigned int size0 = reader0.count();
							if (size0 >0)
							{
								compack::Iterator it0;
								if (it.template isIsoArray<uint32_t>())
								{
									(*(qsort_sort_paramsCompack<BUFFER> *)&*i)._m_indata.push_back(it.template getAsIsoArray<uint32_t>(), it.getIsoArrayLength());
								}
								else
								{
									(*(qsort_sort_paramsCompack<BUFFER> *)&*i)._m_indata.resize(size0);
									for (unsigned int i0 = 0; i0 < (unsigned int)size0; i0++)
									{
										if (!reader0.next(it0))
										{
											throw Lsc::ParseErrorException() << BSL_EARG << "bad data" << "indata";
										}
										(*(qsort_sort_paramsCompack<BUFFER> *)&*i)._m_indata.unsafe_get(i0) = it0.template getAsNum<uint32_t>();
									}
								}
							}
						}
						__count ++;
						continue;
					}
				}
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		static void save(idm::vector<qsort_sort_params> & vec, compack::ObjectIsoArrayWriter<BUFFER> * object)
		{
			{
				ArrayWriter<BUFFER> writer(object, "indata", sizeof("indata"));
				for (idm::vector<qsort_sort_params>::iterator i = vec.begin(); i != vec.end(); ++i)
				{
					ArrayWriter<BUFFER> writer0(&writer);
					for (unsigned int i0 = 0; i0 < (*(qsort_sort_paramsCompack<BUFFER> *)&*i)._m_indata.size(); i0++)
					{
						writer0.template putNum<uint32_t>((*(qsort_sort_paramsCompack<BUFFER> *)&*i)._m_indata.unsafe_get(i0));
					}
				}
			}
		}
		
		void save(compack::ObjectWriter<BUFFER> * object)
		{
			{
				ArrayWriter<BUFFER> writer0(object, "indata", sizeof("indata"));
				for (unsigned int i0 = 0; i0 < _m_indata.size(); i0++)
				{
					writer0.template putNum<uint32_t>(_m_indata.unsafe_get(i0));
				}
			}
			for (unsigned int i=0; i<unknown_size(); i++)
			{
				object->putItem(unknown().template get<ObjectIterator>(i));
			}
		}
	private:
		
		qsort_sort_paramsCompack() : qsort_sort_params(0){};
	};
	template<typename BUFFER>class qsort_sort_result_paramsCompack : plclic qsort_sort_result_params
	{
	plclic:
		
		void load(compack::Reader<BUFFER> & reader)
		try
		{
			clear();
			int __count = 0;
			ObjectIterator it;
			while (reader.next(it))
			{
				if (!strcmp(it.getName(), "outdata"))
				{
					compack::Reader<BUFFER> reader0(reader, it);
					unsigned int size0 = reader0.count();
					if (size0 >0)
					{
						compack::Iterator it0;
						if (it.template isIsoArray<uint32_t>())
						{
							_m_outdata.push_back(it.template getAsIsoArray<uint32_t>(), it.getIsoArrayLength());
						}
						else
						{
							_m_outdata.resize(size0);
							for (unsigned int i0 = 0; i0 < (unsigned int)size0; i0++)
							{
								if (!reader0.next(it0))
								{
									throw Lsc::ParseErrorException() << BSL_EARG << "bad data" << "outdata";
								}
								_m_outdata.unsafe_get(i0) = it0.template getAsNum<uint32_t>();
							}
						}
					}
					__count ++;
					continue;
				}
				unknown().template push_back<ObjectIterator>(it);
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		catch (...)
		{
			clear();
			throw;
		}
		
		static void load(Lsc::mempool *, idm::vector<qsort_sort_result_params> & vec, compack::Reader<BUFFER> & arrayreader)
		{
			int __count = 0;
			ObjectIterator arrayit;
			compack::Reader<BUFFER> resizereader = arrayreader;
			if (resizereader.next(arrayit))
			{
				compack::Reader<BUFFER> resizereader1(resizereader, arrayit);
				vec.resize(resizereader1.count());
				while (arrayreader.next(arrayit))
				{
					if (!strcmp(arrayit.getName(), "outdata"))
					{
						Iterator it;
						compack::Reader<BUFFER> reader(arrayreader, arrayit);
						for (idm::vector<qsort_sort_result_params>::iterator i = vec.begin(); i != vec.end(); ++i)
						{
							if (!reader.next(it))
							{
								throw Lsc::ParseErrorException() << BSL_EARG << "bad data";
							}
							compack::Reader<BUFFER> reader0(reader, it);
							unsigned int size0 = reader0.count();
							if (size0 >0)
							{
								compack::Iterator it0;
								if (it.template isIsoArray<uint32_t>())
								{
									(*(qsort_sort_result_paramsCompack<BUFFER> *)&*i)._m_outdata.push_back(it.template getAsIsoArray<uint32_t>(), it.getIsoArrayLength());
								}
								else
								{
									(*(qsort_sort_result_paramsCompack<BUFFER> *)&*i)._m_outdata.resize(size0);
									for (unsigned int i0 = 0; i0 < (unsigned int)size0; i0++)
									{
										if (!reader0.next(it0))
										{
											throw Lsc::ParseErrorException() << BSL_EARG << "bad data" << "outdata";
										}
										(*(qsort_sort_result_paramsCompack<BUFFER> *)&*i)._m_outdata.unsafe_get(i0) = it0.template getAsNum<uint32_t>();
									}
								}
							}
						}
						__count ++;
						continue;
					}
				}
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		static void save(idm::vector<qsort_sort_result_params> & vec, compack::ObjectIsoArrayWriter<BUFFER> * object)
		{
			{
				ArrayWriter<BUFFER> writer(object, "outdata", sizeof("outdata"));
				for (idm::vector<qsort_sort_result_params>::iterator i = vec.begin(); i != vec.end(); ++i)
				{
					ArrayWriter<BUFFER> writer0(&writer);
					for (unsigned int i0 = 0; i0 < (*(qsort_sort_result_paramsCompack<BUFFER> *)&*i)._m_outdata.size(); i0++)
					{
						writer0.template putNum<uint32_t>((*(qsort_sort_result_paramsCompack<BUFFER> *)&*i)._m_outdata.unsafe_get(i0));
					}
				}
			}
		}
		
		void save(compack::ObjectWriter<BUFFER> * object)
		{
			{
				ArrayWriter<BUFFER> writer0(object, "outdata", sizeof("outdata"));
				for (unsigned int i0 = 0; i0 < _m_outdata.size(); i0++)
				{
					writer0.template putNum<uint32_t>(_m_outdata.unsafe_get(i0));
				}
			}
			for (unsigned int i=0; i<unknown_size(); i++)
			{
				object->putItem(unknown().template get<ObjectIterator>(i));
			}
		}
	private:
		
		qsort_sort_result_paramsCompack() : qsort_sort_result_params(0){};
	};
	template<typename BUFFER>class qsort_sort_responseCompack : plclic qsort_sort_response
	{
	plclic:
		
		void load(compack::Reader<BUFFER> & reader)
		try
		{
			clear();
			int __count = 0;
			ObjectIterator it;
			while (reader.next(it))
			{
				if (!strcmp(it.getName(), "result_params"))
				{
					compack::Reader<BUFFER> newobject(reader, it);
					static_cast<qsort_sort_result_paramsCompack<BUFFER> *>(_m_result_params)->load(newobject);
					__count ++;
					continue;
				}
				unknown().template push_back<ObjectIterator>(it);
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		catch (...)
		{
			clear();
			throw;
		}
		
		static void load(Lsc::mempool *, idm::vector<qsort_sort_response> & vec, compack::Reader<BUFFER> & arrayreader)
		{
			int __count = 0;
			ObjectIterator arrayit;
			compack::Reader<BUFFER> resizereader = arrayreader;
			if (resizereader.next(arrayit))
			{
				compack::Reader<BUFFER> resizereader1(resizereader, arrayit);
				vec.resize(resizereader1.count());
				while (arrayreader.next(arrayit))
				{
					if (!strcmp(arrayit.getName(), "result_params"))
					{
						Iterator it;
						compack::Reader<BUFFER> reader(arrayreader, arrayit);
						for (idm::vector<qsort_sort_response>::iterator i = vec.begin(); i != vec.end(); ++i)
						{
							if (!reader.next(it))
							{
								throw Lsc::ParseErrorException() << BSL_EARG << "bad data";
							}
							compack::Reader<BUFFER> newobject(reader, it);
							static_cast<qsort_sort_result_paramsCompack<BUFFER> *>((*(qsort_sort_responseCompack<BUFFER> *)&*i)._m_result_params)->load(newobject);
						}
						__count ++;
						continue;
					}
				}
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		static void save(idm::vector<qsort_sort_response> & vec, compack::ObjectIsoArrayWriter<BUFFER> * object)
		{
			{
				ArrayWriter<BUFFER> writer(object, "result_params", sizeof("result_params"));
				for (idm::vector<qsort_sort_response>::iterator i = vec.begin(); i != vec.end(); ++i)
				{
					ObjectWriter<BUFFER> slcwriter(&writer);
					(*(qsort_sort_responseCompack<BUFFER> *)&*i)._m_result_params->save(&slcwriter);
				}
			}
		}
		
		void save(compack::ObjectWriter<BUFFER> * object)
		{
			if (_m_result_params!= 0)
			{
				compack::ObjectWriter<BUFFER> newobject(object, "result_params", sizeof("result_params"));
				static_cast<qsort_sort_result_paramsCompack<BUFFER> *>(_m_result_params)->save(&newobject);
			}
			for (unsigned int i=0; i<unknown_size(); i++)
			{
				object->putItem(unknown().template get<ObjectIterator>(i));
			}
		}
	private:
		
		qsort_sort_responseCompack() : qsort_sort_response(0){};
	};
	template<typename BUFFER>class qsort_echo_paramsCompack : plclic qsort_echo_params
	{
	plclic:
		
		void load(compack::Reader<BUFFER> & reader)
		try
		{
			clear();
			int __count = 0;
			ObjectIterator it;
			while (reader.next(it))
			{
				if (!strcmp(it.getName(), "instr"))
				{
					_m_instr.set_ptr(it.getAsString());
					__count ++;
					continue;
				}
				unknown().template push_back<ObjectIterator>(it);
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		catch (...)
		{
			clear();
			throw;
		}
		
		static void load(Lsc::mempool *, idm::vector<qsort_echo_params> & vec, compack::Reader<BUFFER> & arrayreader)
		{
			int __count = 0;
			ObjectIterator arrayit;
			compack::Reader<BUFFER> resizereader = arrayreader;
			if (resizereader.next(arrayit))
			{
				compack::Reader<BUFFER> resizereader1(resizereader, arrayit);
				vec.resize(resizereader1.count());
				while (arrayreader.next(arrayit))
				{
					if (!strcmp(arrayit.getName(), "instr"))
					{
						Iterator it;
						compack::Reader<BUFFER> reader(arrayreader, arrayit);
						for (idm::vector<qsort_echo_params>::iterator i = vec.begin(); i != vec.end(); ++i)
						{
							if (!reader.next(it))
							{
								throw Lsc::ParseErrorException() << BSL_EARG << "bad data";
							}
							(*(qsort_echo_paramsCompack<BUFFER> *)&*i)._m_instr.set_ptr(it.getAsString());
						}
						__count ++;
						continue;
					}
				}
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		static void save(idm::vector<qsort_echo_params> & vec, compack::ObjectIsoArrayWriter<BUFFER> * object)
		{
			{
				ArrayWriter<BUFFER> writer(object, "instr", sizeof("instr"));
				for (idm::vector<qsort_echo_params>::iterator i = vec.begin(); i != vec.end(); ++i)
				{
					writer.putString((*(qsort_echo_paramsCompack<BUFFER> *)&*i)._m_instr.get(), (*(qsort_echo_paramsCompack<BUFFER> *)&*i)._m_instr.length() + 1);
				}
			}
		}
		
		void save(compack::ObjectWriter<BUFFER> * object)
		{
			object->putString("instr", sizeof("instr"), _m_instr.get(), _m_instr.length() + 1);
			for (unsigned int i=0; i<unknown_size(); i++)
			{
				object->putItem(unknown().template get<ObjectIterator>(i));
			}
		}
	private:
		
		qsort_echo_paramsCompack() : qsort_echo_params(0){};
	};
	template<typename BUFFER>class qsort_echo_result_paramsCompack : plclic qsort_echo_result_params
	{
	plclic:
		
		void load(compack::Reader<BUFFER> & reader)
		try
		{
			clear();
			int __count = 0;
			ObjectIterator it;
			while (reader.next(it))
			{
				if (!strcmp(it.getName(), "outstr"))
				{
					_m_outstr.set_ptr(it.getAsString());
					__count ++;
					continue;
				}
				unknown().template push_back<ObjectIterator>(it);
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		catch (...)
		{
			clear();
			throw;
		}
		
		static void load(Lsc::mempool *, idm::vector<qsort_echo_result_params> & vec, compack::Reader<BUFFER> & arrayreader)
		{
			int __count = 0;
			ObjectIterator arrayit;
			compack::Reader<BUFFER> resizereader = arrayreader;
			if (resizereader.next(arrayit))
			{
				compack::Reader<BUFFER> resizereader1(resizereader, arrayit);
				vec.resize(resizereader1.count());
				while (arrayreader.next(arrayit))
				{
					if (!strcmp(arrayit.getName(), "outstr"))
					{
						Iterator it;
						compack::Reader<BUFFER> reader(arrayreader, arrayit);
						for (idm::vector<qsort_echo_result_params>::iterator i = vec.begin(); i != vec.end(); ++i)
						{
							if (!reader.next(it))
							{
								throw Lsc::ParseErrorException() << BSL_EARG << "bad data";
							}
							(*(qsort_echo_result_paramsCompack<BUFFER> *)&*i)._m_outstr.set_ptr(it.getAsString());
						}
						__count ++;
						continue;
					}
				}
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		static void save(idm::vector<qsort_echo_result_params> & vec, compack::ObjectIsoArrayWriter<BUFFER> * object)
		{
			{
				ArrayWriter<BUFFER> writer(object, "outstr", sizeof("outstr"));
				for (idm::vector<qsort_echo_result_params>::iterator i = vec.begin(); i != vec.end(); ++i)
				{
					writer.putString((*(qsort_echo_result_paramsCompack<BUFFER> *)&*i)._m_outstr.get(), (*(qsort_echo_result_paramsCompack<BUFFER> *)&*i)._m_outstr.length() + 1);
				}
			}
		}
		
		void save(compack::ObjectWriter<BUFFER> * object)
		{
			object->putString("outstr", sizeof("outstr"), _m_outstr.get(), _m_outstr.length() + 1);
			for (unsigned int i=0; i<unknown_size(); i++)
			{
				object->putItem(unknown().template get<ObjectIterator>(i));
			}
		}
	private:
		
		qsort_echo_result_paramsCompack() : qsort_echo_result_params(0){};
	};
	template<typename BUFFER>class qsort_echo_responseCompack : plclic qsort_echo_response
	{
	plclic:
		
		void load(compack::Reader<BUFFER> & reader)
		try
		{
			clear();
			int __count = 0;
			ObjectIterator it;
			while (reader.next(it))
			{
				if (!strcmp(it.getName(), "result_params"))
				{
					compack::Reader<BUFFER> newobject(reader, it);
					static_cast<qsort_echo_result_paramsCompack<BUFFER> *>(_m_result_params)->load(newobject);
					__count ++;
					continue;
				}
				unknown().template push_back<ObjectIterator>(it);
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		catch (...)
		{
			clear();
			throw;
		}
		
		static void load(Lsc::mempool *, idm::vector<qsort_echo_response> & vec, compack::Reader<BUFFER> & arrayreader)
		{
			int __count = 0;
			ObjectIterator arrayit;
			compack::Reader<BUFFER> resizereader = arrayreader;
			if (resizereader.next(arrayit))
			{
				compack::Reader<BUFFER> resizereader1(resizereader, arrayit);
				vec.resize(resizereader1.count());
				while (arrayreader.next(arrayit))
				{
					if (!strcmp(arrayit.getName(), "result_params"))
					{
						Iterator it;
						compack::Reader<BUFFER> reader(arrayreader, arrayit);
						for (idm::vector<qsort_echo_response>::iterator i = vec.begin(); i != vec.end(); ++i)
						{
							if (!reader.next(it))
							{
								throw Lsc::ParseErrorException() << BSL_EARG << "bad data";
							}
							compack::Reader<BUFFER> newobject(reader, it);
							static_cast<qsort_echo_result_paramsCompack<BUFFER> *>((*(qsort_echo_responseCompack<BUFFER> *)&*i)._m_result_params)->load(newobject);
						}
						__count ++;
						continue;
					}
				}
			}
			if (__count != 1)
			{
				throw Lsc::KeyNotFoundException() << BSL_EARG << "key not found";
			}
		}
		static void save(idm::vector<qsort_echo_response> & vec, compack::ObjectIsoArrayWriter<BUFFER> * object)
		{
			{
				ArrayWriter<BUFFER> writer(object, "result_params", sizeof("result_params"));
				for (idm::vector<qsort_echo_response>::iterator i = vec.begin(); i != vec.end(); ++i)
				{
					ObjectWriter<BUFFER> slcwriter(&writer);
					(*(qsort_echo_responseCompack<BUFFER> *)&*i)._m_result_params->save(&slcwriter);
				}
			}
		}
		
		void save(compack::ObjectWriter<BUFFER> * object)
		{
			if (_m_result_params!= 0)
			{
				compack::ObjectWriter<BUFFER> newobject(object, "result_params", sizeof("result_params"));
				static_cast<qsort_echo_result_paramsCompack<BUFFER> *>(_m_result_params)->save(&newobject);
			}
			for (unsigned int i=0; i<unknown_size(); i++)
			{
				object->putItem(unknown().template get<ObjectIterator>(i));
			}
		}
	private:
		
		qsort_echo_responseCompack() : qsort_echo_response(0){};
	};
}
namespace idm
{
	using namespace idm;
	const idm::vector<uint32_t> & qsort_sort_params::indata() const
	{
		return _m_indata;
	}
	idm::vector<uint32_t> * qsort_sort_params::m_indata()
	{
		return &_m_indata;
	}
	uint32_t qsort_sort_params::indata(unsigned int i0) const
	{
		return _m_indata.get(i0);
	}
	unsigned int qsort_sort_params::indata_size() const
	{
		return _m_indata.size();
	}
	void qsort_sort_params::set_indata(unsigned int i0, uint32_t _value)
	{
		_m_indata[i0] = _value;
	}
	bool qsort_sort_params::has_indata() const
	{
		return true;
	}
	void qsort_sort_params::clear_indata()
	{
		_m_indata.clear();
	}
	qsort_sort_params::qsort_sort_params(Lsc::mempool * pool ) : idm::CompackBean(pool), _m_indata(_pool), _unknown(0)
	{
		if (0 == _pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG << "pool should not be zero";
		}
	}
	qsort_sort_params::qsort_sort_params(const qsort_sort_params& _value): idm::CompackBean(_value), _m_indata(_pool), _unknown(0)
	{
		assign(_value);
	}
	void qsort_sort_params::assign(const qsort_sort_params& _value)
	{
		_m_indata = _value._m_indata;
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		if (_value._unknown != 0)
		{
			unknown() = _value.unknown();
		}
	}
	qsort_sort_params & qsort_sort_params::operator =(const qsort_sort_params& _value)
	{
		assign(_value);
		return *this;
	}
	qsort_sort_params::~qsort_sort_params()
	{
		if (_unknown != 0)
		{
			_unknown->~Unknown();
			_pool->free(_unknown, sizeof(*_unknown));
		}
	}
	void qsort_sort_params::clear()
	{
		_m_indata.clear();
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->clear();
		}
	}
	void qsort_sort_params::detach()
	{
		_m_indata.detach();
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->detach();
		}
		if (_unknown != 0)
		{
			_unknown->clear();
		}
	}
	void qsort_sort_params::load(const void *buffer, unsigned int size)
	{
		void *ptr = const_cast<void *>(buffer);
		compack::buffer::buffer buf(reinterpret_cast<char *>(ptr), size);
		compack::buffer::Reader reader(buf);
		(static_cast<qsort_sort_paramsCompack<compack::buffer::buffer> *>(this))->qsort_sort_paramsCompack<compack::buffer::buffer>::load(reader);
	}
	void qsort_sort_params::save(void *buffer, unsigned int size)
	{
		compack::buffer::buffer buf(buffer, size);
		compack::buffer::Object writer(&buf);
		(static_cast<qsort_sort_paramsCompack<compack::buffer::buffer> *>(this))->qsort_sort_paramsCompack<compack::buffer::buffer>::save(&writer);
	}
	void qsort_sort_params::save(compack::buffer::Object *object)
	{
		(static_cast<qsort_sort_paramsCompack<compack::buffer::buffer> *>(this))->qsort_sort_paramsCompack<compack::buffer::buffer>::save(object);
	}
	idm::Unknown & qsort_sort_params::unknown()
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	const idm::Unknown & qsort_sort_params::unknown() const
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	unsigned int qsort_sort_params::unknown_size() const
	{
		if (_unknown == 0)
		{
			return 0;
		}
		else
		{
			return _unknown->size();
		}
	}
	const idm::vector<uint32_t> & qsort_sort_result_params::outdata() const
	{
		return _m_outdata;
	}
	idm::vector<uint32_t> * qsort_sort_result_params::m_outdata()
	{
		return &_m_outdata;
	}
	uint32_t qsort_sort_result_params::outdata(unsigned int i0) const
	{
		return _m_outdata.get(i0);
	}
	unsigned int qsort_sort_result_params::outdata_size() const
	{
		return _m_outdata.size();
	}
	void qsort_sort_result_params::set_outdata(unsigned int i0, uint32_t _value)
	{
		_m_outdata[i0] = _value;
	}
	bool qsort_sort_result_params::has_outdata() const
	{
		return true;
	}
	void qsort_sort_result_params::clear_outdata()
	{
		_m_outdata.clear();
	}
	qsort_sort_result_params::qsort_sort_result_params(Lsc::mempool * pool ) : idm::CompackBean(pool), _m_outdata(_pool), _unknown(0)
	{
		if (0 == _pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG << "pool should not be zero";
		}
	}
	qsort_sort_result_params::qsort_sort_result_params(const qsort_sort_result_params& _value): idm::CompackBean(_value), _m_outdata(_pool), _unknown(0)
	{
		assign(_value);
	}
	void qsort_sort_result_params::assign(const qsort_sort_result_params& _value)
	{
		_m_outdata = _value._m_outdata;
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		if (_value._unknown != 0)
		{
			unknown() = _value.unknown();
		}
	}
	qsort_sort_result_params & qsort_sort_result_params::operator =(const qsort_sort_result_params& _value)
	{
		assign(_value);
		return *this;
	}
	qsort_sort_result_params::~qsort_sort_result_params()
	{
		if (_unknown != 0)
		{
			_unknown->~Unknown();
			_pool->free(_unknown, sizeof(*_unknown));
		}
	}
	void qsort_sort_result_params::clear()
	{
		_m_outdata.clear();
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->clear();
		}
	}
	void qsort_sort_result_params::detach()
	{
		_m_outdata.detach();
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->detach();
		}
		if (_unknown != 0)
		{
			_unknown->clear();
		}
	}
	void qsort_sort_result_params::load(const void *buffer, unsigned int size)
	{
		void *ptr = const_cast<void *>(buffer);
		compack::buffer::buffer buf(reinterpret_cast<char *>(ptr), size);
		compack::buffer::Reader reader(buf);
		(static_cast<qsort_sort_result_paramsCompack<compack::buffer::buffer> *>(this))->qsort_sort_result_paramsCompack<compack::buffer::buffer>::load(reader);
	}
	void qsort_sort_result_params::save(void *buffer, unsigned int size)
	{
		compack::buffer::buffer buf(buffer, size);
		compack::buffer::Object writer(&buf);
		(static_cast<qsort_sort_result_paramsCompack<compack::buffer::buffer> *>(this))->qsort_sort_result_paramsCompack<compack::buffer::buffer>::save(&writer);
	}
	void qsort_sort_result_params::save(compack::buffer::Object *object)
	{
		(static_cast<qsort_sort_result_paramsCompack<compack::buffer::buffer> *>(this))->qsort_sort_result_paramsCompack<compack::buffer::buffer>::save(object);
	}
	idm::Unknown & qsort_sort_result_params::unknown()
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	const idm::Unknown & qsort_sort_result_params::unknown() const
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	unsigned int qsort_sort_result_params::unknown_size() const
	{
		if (_unknown == 0)
		{
			return 0;
		}
		else
		{
			return _unknown->size();
		}
	}
	const qsort_sort_result_params & qsort_sort_response::result_params() const
	{
		return *_m_result_params;
	}
	qsort_sort_result_params* qsort_sort_response::m_result_params()
	{
		return _m_result_params;
	}
	bool qsort_sort_response::has_result_params() const
	{
		return true;
	}
	void qsort_sort_response::clear_result_params()
	{
		if (_m_result_params != 0)
		{
			_m_result_params ->clear();
		}
	}
	qsort_sort_response::qsort_sort_response(Lsc::mempool * pool ) : idm::CompackBean(pool), _m_result_params(0), _unknown(0)
	{
		if (0 == _pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG << "pool should not be zero";
		}
		_m_result_params = qsort_sort_result_params::create(_pool);
	}
	qsort_sort_response::qsort_sort_response(const qsort_sort_response& _value): idm::CompackBean(_value), _m_result_params(0), _unknown(0)
	{
		_m_result_params = qsort_sort_result_params::create(_pool);
		assign(_value);
	}
	void qsort_sort_response::assign(const qsort_sort_response& _value)
	{
		* _m_result_params = * _value._m_result_params;
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		if (_value._unknown != 0)
		{
			unknown() = _value.unknown();
		}
	}
	qsort_sort_response & qsort_sort_response::operator =(const qsort_sort_response& _value)
	{
		assign(_value);
		return *this;
	}
	qsort_sort_response::~qsort_sort_response()
	{
		if (_m_result_params != 0)
		{
			qsort_sort_result_params::destroy(_m_result_params);
			_m_result_params = 0;
		}
		if (_unknown != 0)
		{
			_unknown->~Unknown();
			_pool->free(_unknown, sizeof(*_unknown));
		}
	}
	void qsort_sort_response::clear()
	{
		_m_result_params->clear();
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->clear();
		}
	}
	void qsort_sort_response::detach()
	{
		_m_result_params->detach();
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->detach();
		}
		if (_unknown != 0)
		{
			_unknown->clear();
		}
	}
	void qsort_sort_response::load(const void *buffer, unsigned int size)
	{
		void *ptr = const_cast<void *>(buffer);
		compack::buffer::buffer buf(reinterpret_cast<char *>(ptr), size);
		compack::buffer::Reader reader(buf);
		(static_cast<qsort_sort_responseCompack<compack::buffer::buffer> *>(this))->qsort_sort_responseCompack<compack::buffer::buffer>::load(reader);
	}
	void qsort_sort_response::save(void *buffer, unsigned int size)
	{
		compack::buffer::buffer buf(buffer, size);
		compack::buffer::Object writer(&buf);
		(static_cast<qsort_sort_responseCompack<compack::buffer::buffer> *>(this))->qsort_sort_responseCompack<compack::buffer::buffer>::save(&writer);
	}
	void qsort_sort_response::save(compack::buffer::Object *object)
	{
		(static_cast<qsort_sort_responseCompack<compack::buffer::buffer> *>(this))->qsort_sort_responseCompack<compack::buffer::buffer>::save(object);
	}
	idm::Unknown & qsort_sort_response::unknown()
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	const idm::Unknown & qsort_sort_response::unknown() const
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	unsigned int qsort_sort_response::unknown_size() const
	{
		if (_unknown == 0)
		{
			return 0;
		}
		else
		{
			return _unknown->size();
		}
	}
	const char * qsort_echo_params::instr(unsigned int * length) const
	{
		return _m_instr.get(length);
	}
	void qsort_echo_params::set_instr(const char * _str, int _length)
	{
		_m_instr.set(_str, _length);
	}
	void qsort_echo_params::set_ptr_instr(const char * _str, int)
	{
		_m_instr.set_ptr(_str);
	}
	bool qsort_echo_params::has_instr() const
	{
		return true;
	}
	void qsort_echo_params::clear_instr()
	{
		_m_instr.clear();
	}
	qsort_echo_params::qsort_echo_params(Lsc::mempool * pool ) : idm::CompackBean(pool), _m_instr(_pool), _unknown(0)
	{
		if (0 == _pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG << "pool should not be zero";
		}
	}
	qsort_echo_params::qsort_echo_params(const qsort_echo_params& _value): idm::CompackBean(_value), _m_instr(_pool), _unknown(0)
	{
		assign(_value);
	}
	void qsort_echo_params::assign(const qsort_echo_params& _value)
	{
		_m_instr = _value._m_instr;
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		if (_value._unknown != 0)
		{
			unknown() = _value.unknown();
		}
	}
	qsort_echo_params & qsort_echo_params::operator =(const qsort_echo_params& _value)
	{
		assign(_value);
		return *this;
	}
	qsort_echo_params::~qsort_echo_params()
	{
		if (_unknown != 0)
		{
			_unknown->~Unknown();
			_pool->free(_unknown, sizeof(*_unknown));
		}
	}
	void qsort_echo_params::clear()
	{
		_m_instr.clear();
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->clear();
		}
	}
	void qsort_echo_params::detach()
	{
		_m_instr.detach();
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->detach();
		}
		if (_unknown != 0)
		{
			_unknown->clear();
		}
	}
	void qsort_echo_params::load(const void *buffer, unsigned int size)
	{
		void *ptr = const_cast<void *>(buffer);
		compack::buffer::buffer buf(reinterpret_cast<char *>(ptr), size);
		compack::buffer::Reader reader(buf);
		(static_cast<qsort_echo_paramsCompack<compack::buffer::buffer> *>(this))->qsort_echo_paramsCompack<compack::buffer::buffer>::load(reader);
	}
	void qsort_echo_params::save(void *buffer, unsigned int size)
	{
		compack::buffer::buffer buf(buffer, size);
		compack::buffer::Object writer(&buf);
		(static_cast<qsort_echo_paramsCompack<compack::buffer::buffer> *>(this))->qsort_echo_paramsCompack<compack::buffer::buffer>::save(&writer);
	}
	void qsort_echo_params::save(compack::buffer::Object *object)
	{
		(static_cast<qsort_echo_paramsCompack<compack::buffer::buffer> *>(this))->qsort_echo_paramsCompack<compack::buffer::buffer>::save(object);
	}
	idm::Unknown & qsort_echo_params::unknown()
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	const idm::Unknown & qsort_echo_params::unknown() const
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	unsigned int qsort_echo_params::unknown_size() const
	{
		if (_unknown == 0)
		{
			return 0;
		}
		else
		{
			return _unknown->size();
		}
	}
	const char * qsort_echo_result_params::outstr(unsigned int * length) const
	{
		return _m_outstr.get(length);
	}
	void qsort_echo_result_params::set_outstr(const char * _str, int _length)
	{
		_m_outstr.set(_str, _length);
	}
	void qsort_echo_result_params::set_ptr_outstr(const char * _str, int)
	{
		_m_outstr.set_ptr(_str);
	}
	bool qsort_echo_result_params::has_outstr() const
	{
		return true;
	}
	void qsort_echo_result_params::clear_outstr()
	{
		_m_outstr.clear();
	}
	qsort_echo_result_params::qsort_echo_result_params(Lsc::mempool * pool ) : idm::CompackBean(pool), _m_outstr(_pool), _unknown(0)
	{
		if (0 == _pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG << "pool should not be zero";
		}
	}
	qsort_echo_result_params::qsort_echo_result_params(const qsort_echo_result_params& _value): idm::CompackBean(_value), _m_outstr(_pool), _unknown(0)
	{
		assign(_value);
	}
	void qsort_echo_result_params::assign(const qsort_echo_result_params& _value)
	{
		_m_outstr = _value._m_outstr;
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		if (_value._unknown != 0)
		{
			unknown() = _value.unknown();
		}
	}
	qsort_echo_result_params & qsort_echo_result_params::operator =(const qsort_echo_result_params& _value)
	{
		assign(_value);
		return *this;
	}
	qsort_echo_result_params::~qsort_echo_result_params()
	{
		if (_unknown != 0)
		{
			_unknown->~Unknown();
			_pool->free(_unknown, sizeof(*_unknown));
		}
	}
	void qsort_echo_result_params::clear()
	{
		_m_outstr.clear();
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->clear();
		}
	}
	void qsort_echo_result_params::detach()
	{
		_m_outstr.detach();
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->detach();
		}
		if (_unknown != 0)
		{
			_unknown->clear();
		}
	}
	void qsort_echo_result_params::load(const void *buffer, unsigned int size)
	{
		void *ptr = const_cast<void *>(buffer);
		compack::buffer::buffer buf(reinterpret_cast<char *>(ptr), size);
		compack::buffer::Reader reader(buf);
		(static_cast<qsort_echo_result_paramsCompack<compack::buffer::buffer> *>(this))->qsort_echo_result_paramsCompack<compack::buffer::buffer>::load(reader);
	}
	void qsort_echo_result_params::save(void *buffer, unsigned int size)
	{
		compack::buffer::buffer buf(buffer, size);
		compack::buffer::Object writer(&buf);
		(static_cast<qsort_echo_result_paramsCompack<compack::buffer::buffer> *>(this))->qsort_echo_result_paramsCompack<compack::buffer::buffer>::save(&writer);
	}
	void qsort_echo_result_params::save(compack::buffer::Object *object)
	{
		(static_cast<qsort_echo_result_paramsCompack<compack::buffer::buffer> *>(this))->qsort_echo_result_paramsCompack<compack::buffer::buffer>::save(object);
	}
	idm::Unknown & qsort_echo_result_params::unknown()
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	const idm::Unknown & qsort_echo_result_params::unknown() const
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	unsigned int qsort_echo_result_params::unknown_size() const
	{
		if (_unknown == 0)
		{
			return 0;
		}
		else
		{
			return _unknown->size();
		}
	}
	const qsort_echo_result_params & qsort_echo_response::result_params() const
	{
		return *_m_result_params;
	}
	qsort_echo_result_params* qsort_echo_response::m_result_params()
	{
		return _m_result_params;
	}
	bool qsort_echo_response::has_result_params() const
	{
		return true;
	}
	void qsort_echo_response::clear_result_params()
	{
		if (_m_result_params != 0)
		{
			_m_result_params ->clear();
		}
	}
	qsort_echo_response::qsort_echo_response(Lsc::mempool * pool ) : idm::CompackBean(pool), _m_result_params(0), _unknown(0)
	{
		if (0 == _pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG << "pool should not be zero";
		}
		_m_result_params = qsort_echo_result_params::create(_pool);
	}
	qsort_echo_response::qsort_echo_response(const qsort_echo_response& _value): idm::CompackBean(_value), _m_result_params(0), _unknown(0)
	{
		_m_result_params = qsort_echo_result_params::create(_pool);
		assign(_value);
	}
	void qsort_echo_response::assign(const qsort_echo_response& _value)
	{
		* _m_result_params = * _value._m_result_params;
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		if (_value._unknown != 0)
		{
			unknown() = _value.unknown();
		}
	}
	qsort_echo_response & qsort_echo_response::operator =(const qsort_echo_response& _value)
	{
		assign(_value);
		return *this;
	}
	qsort_echo_response::~qsort_echo_response()
	{
		if (_m_result_params != 0)
		{
			qsort_echo_result_params::destroy(_m_result_params);
			_m_result_params = 0;
		}
		if (_unknown != 0)
		{
			_unknown->~Unknown();
			_pool->free(_unknown, sizeof(*_unknown));
		}
	}
	void qsort_echo_response::clear()
	{
		_m_result_params->clear();
		if (_unknown != 0)
		{
			_unknown->clear();
		}
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->clear();
		}
	}
	void qsort_echo_response::detach()
	{
		_m_result_params->detach();
		for (unsigned int i=0; i<_ext.size(); i++)
		{
			_ext[i]->detach();
		}
		if (_unknown != 0)
		{
			_unknown->clear();
		}
	}
	void qsort_echo_response::load(const void *buffer, unsigned int size)
	{
		void *ptr = const_cast<void *>(buffer);
		compack::buffer::buffer buf(reinterpret_cast<char *>(ptr), size);
		compack::buffer::Reader reader(buf);
		(static_cast<qsort_echo_responseCompack<compack::buffer::buffer> *>(this))->qsort_echo_responseCompack<compack::buffer::buffer>::load(reader);
	}
	void qsort_echo_response::save(void *buffer, unsigned int size)
	{
		compack::buffer::buffer buf(buffer, size);
		compack::buffer::Object writer(&buf);
		(static_cast<qsort_echo_responseCompack<compack::buffer::buffer> *>(this))->qsort_echo_responseCompack<compack::buffer::buffer>::save(&writer);
	}
	void qsort_echo_response::save(compack::buffer::Object *object)
	{
		(static_cast<qsort_echo_responseCompack<compack::buffer::buffer> *>(this))->qsort_echo_responseCompack<compack::buffer::buffer>::save(object);
	}
	idm::Unknown & qsort_echo_response::unknown()
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	const idm::Unknown & qsort_echo_response::unknown() const
	{
		if (_unknown == 0)
		{
			_unknown = (idm::Unknown *)_pool->malloc(sizeof(idm::Unknown));
			new (_unknown) idm::Unknown(_pool);
		}
		return *_unknown;
	}
	unsigned int qsort_echo_response::unknown_size() const
	{
		if (_unknown == 0)
		{
			return 0;
		}
		else
		{
			return _unknown->size();
		}
	}
	qsort_sort_params * qsort_sort_params::create(Lsc::mempool *pool)
	{
		if (0 == pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG;
		}
		qsort_sort_params * tmp = (qsort_sort_params*) pool->malloc(sizeof(qsort_sort_params));
		if (0 == tmp)
		{
			throw Lsc::BadAllocException() << BSL_EARG;
		}
		new (tmp)qsort_sort_params(pool);
		return tmp;
	}
	qsort_sort_result_params * qsort_sort_result_params::create(Lsc::mempool *pool)
	{
		if (0 == pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG;
		}
		qsort_sort_result_params * tmp = (qsort_sort_result_params*) pool->malloc(sizeof(qsort_sort_result_params));
		if (0 == tmp)
		{
			throw Lsc::BadAllocException() << BSL_EARG;
		}
		new (tmp)qsort_sort_result_params(pool);
		return tmp;
	}
	qsort_sort_response * qsort_sort_response::create(Lsc::mempool *pool)
	{
		if (0 == pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG;
		}
		qsort_sort_response * tmp = (qsort_sort_response*) pool->malloc(sizeof(qsort_sort_response));
		if (0 == tmp)
		{
			throw Lsc::BadAllocException() << BSL_EARG;
		}
		new (tmp)qsort_sort_response(pool);
		return tmp;
	}
	qsort_echo_params * qsort_echo_params::create(Lsc::mempool *pool)
	{
		if (0 == pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG;
		}
		qsort_echo_params * tmp = (qsort_echo_params*) pool->malloc(sizeof(qsort_echo_params));
		if (0 == tmp)
		{
			throw Lsc::BadAllocException() << BSL_EARG;
		}
		new (tmp)qsort_echo_params(pool);
		return tmp;
	}
	qsort_echo_result_params * qsort_echo_result_params::create(Lsc::mempool *pool)
	{
		if (0 == pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG;
		}
		qsort_echo_result_params * tmp = (qsort_echo_result_params*) pool->malloc(sizeof(qsort_echo_result_params));
		if (0 == tmp)
		{
			throw Lsc::BadAllocException() << BSL_EARG;
		}
		new (tmp)qsort_echo_result_params(pool);
		return tmp;
	}
	qsort_echo_response * qsort_echo_response::create(Lsc::mempool *pool)
	{
		if (0 == pool)
		{
			throw Lsc::NullPointerException() << BSL_EARG;
		}
		qsort_echo_response * tmp = (qsort_echo_response*) pool->malloc(sizeof(qsort_echo_response));
		if (0 == tmp)
		{
			throw Lsc::BadAllocException() << BSL_EARG;
		}
		new (tmp)qsort_echo_response(pool);
		return tmp;
	}
	void qsort_sort_params::destroy(qsort_sort_params * data)
	{
		if (data != 0)
		{
			Lsc::mempool *pool = data->_pool;
			data->~qsort_sort_params();
			pool->free(data, sizeof(*data));
		}
	}
	void qsort_sort_result_params::destroy(qsort_sort_result_params * data)
	{
		if (data != 0)
		{
			Lsc::mempool *pool = data->_pool;
			data->~qsort_sort_result_params();
			pool->free(data, sizeof(*data));
		}
	}
	void qsort_sort_response::destroy(qsort_sort_response * data)
	{
		if (data != 0)
		{
			Lsc::mempool *pool = data->_pool;
			data->~qsort_sort_response();
			pool->free(data, sizeof(*data));
		}
	}
	void qsort_echo_params::destroy(qsort_echo_params * data)
	{
		if (data != 0)
		{
			Lsc::mempool *pool = data->_pool;
			data->~qsort_echo_params();
			pool->free(data, sizeof(*data));
		}
	}
	void qsort_echo_result_params::destroy(qsort_echo_result_params * data)
	{
		if (data != 0)
		{
			Lsc::mempool *pool = data->_pool;
			data->~qsort_echo_result_params();
			pool->free(data, sizeof(*data));
		}
	}
	void qsort_echo_response::destroy(qsort_echo_response * data)
	{
		if (data != 0)
		{
			Lsc::mempool *pool = data->_pool;
			data->~qsort_echo_response();
			pool->free(data, sizeof(*data));
		}
	}
}
namespace idm
{
}
