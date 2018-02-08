#include "myclient_include.h"


void MyclientRes :: _wrap(MYSQL_RES *res, bool stored)
{
	this->free();
	_res = res;
	_stored = stored;
	if (stored)
	{
		_count = (unsigned)mysql_num_rows(_res);
	}
	else
	{
		_count = 0;
	}
}







MyclientRes& MyclientRes::operator=(const MyclientRes &)
{ 
	return *this;
}


MYSQL_ROW MyclientRes::next()
{
	MYSQL_ROW row = mysql_fetch_row(_res);
	if (row)
	{
		_pos++;
	}
	return row;
}

bool MyclientRes::seek(unsigned pos)
{
	
	if (pos >= _count)
	{
		return false;
	}

	if (pos == _pos)
	{
		return true;
	}

	mysql_data_seek(_res, pos);
	_pos = pos;
	return true;
}

unsigned MyclientRes::tell() const
{
	return _pos;
}



MYSQL_ROW MyclientRes::operator[](size_t index)
{
	
	if(this->_lastRow == NULL || this->_lastRowIndex != index)
	{
		if(index != this->_pos && !this->seek((unsigned)index))
		{
			return NULL;
		}
		this->_lastRowIndex = index;
		this->_lastRow = this->next();
	}

	return this->_lastRow;
}

const char * MyclientRes::getAt(unsigned h_index,
			unsigned v_index)
{
	MYSQL_ROW row;
	row = (*this)[(size_t)h_index];
	if (row && v_index < this->getFieldsCount())
	{
		return row[v_index];
	}
	return NULL;
}


unsigned MyclientRes::getRowsCount() const
{
	return _count;
}

unsigned MyclientRes::getFieldsCount() const
{
	return mysql_num_fields(_res);
}

bool MyclientRes::isSeekable() const
{
	return _stored;
}

void MyclientRes::free()
{
	if (_res != NULL)
	{
		mysql_free_result(_res);
		_res = NULL;
	}
	_pos = 0;
    _count = 0;
	_lastRowIndex = 0;
	_lastRow = NULL;
}

void MyclientRes::walk(MyclientRes_walk_func_t func, void *args)
{
	MYSQL_ROW row;
	while ((row = this->next()) && func(row, args))
		;
}

void MyclientRes::__print()
{
	unsigned fields_count = this->getFieldsCount();
	unsigned rows_count = this->getRowsCount();

	for (unsigned i = 0; i != rows_count; i++)
	{
		printf("%d:\t", i);
		for (unsigned j = 0; j != fields_count; j++)
		{
			printf("%s\t", (*this)[i][j]);
		}
		puts("");
	}
}
MYSQL_RES * MyclientRes::getRawObj()
{
    return _res;
}





