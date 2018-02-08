#include "myclient_utils.h"


const error_desc_data_t err_dest_data[] =
{
{ 0, "ok" },
{-1,"input param error"},
{-2,"filesystem error"},
{-3,"conf param error"},

{-101,"pool init fail"},
{-102,"pool get connection fail"},
{-103,"pool has not init"},

{-201,"connection connect fail"},
{-202,"connection not init"},
{-203,"connection query string escape fail"},
{-204,"connection_query_fail"},
{ -999,""},
};


const char * mc_get_err_dest(int err_no)
{
	
	const error_desc_data_t* err = err_dest_data;
	while (err->err_no!=-999)
	{
		if (err->err_no==err_no )
		{
			return err->err_desc;
		}
		err++;
	}
	return err->err_desc;
}


