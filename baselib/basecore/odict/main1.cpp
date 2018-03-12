
#include "ul_sign.h"
#include "odict.h"



int build_action_add(sodict_build_t *sdb)
{
	char		   name[128];
	unsigned int   value1, value2;
	u_int		  sign1, sign2;
	sodict_snode_t snode;
 
	printf("please enter name and value: ");
	fscanf(stdin, "%s%d%d", name, &value1, &value2);

	creat_sign_fs64(name, strlen(name), &sign1, &sign2);
	snode.sign1 = sign1;
	snode.sign2 = sign2;
	snode.cuint1 = value1;
	snode.cuint2 = value2;

	if (ODB_ADD_OK == odb_add(sdb, &snode, 1))
		printf("insert a node to dictionary OK\n");
	else
		printf("insert a node to dictionary FAIL\n");

	return 0;
}

int build_action_del(sodict_build_t *sdb)
{
	char		   name[128];
	u_int		  sign1, sign2;
	sodict_snode_t snode;
 
	printf("please enter name: ");
	fscanf(stdin, "%s", name);

	creat_sign_fs64(name, strlen(name), &sign1, &sign2);
	snode.sign1 = sign1;
	snode.sign2 = sign2;

	if (ODB_DEL_OK == odb_del(sdb, &snode))
		printf("delete a node from dictionary OK\n");
	else
		printf("delete a node from dictionary FAIL\n");

	return 0;
}

int build_action_mod(sodict_build_t *sdb)
{
	char		   name[128];
	unsigned int   value1, value2;
	u_int		  sign1, sign2;
	sodict_snode_t snode;
 
	printf("please enter name and value: ");
	fscanf(stdin, "%s%d%d", name, &value1, &value2);

	creat_sign_fs64(name, strlen(name), &sign1, &sign2);
	snode.sign1 = sign1;
	snode.sign2 = sign2;
	snode.cuint1 = value1;
	snode.cuint2 = value2;

	if (ODB_MOD_OK == odb_mod(sdb, &snode))
		printf("modify a node in dictionary OK\n");
	else
		printf("modify a node in dictionary FAIL\n");

	return 0;
}

int build_action_seek(sodict_build_t *sdb)
{
	char		   name[128];
	u_int		  sign1, sign2;
	sodict_snode_t snode;
 
	printf("please enter name: ");
	fscanf(stdin, "%s", name);

	creat_sign_fs64(name, strlen(name), &sign1, &sign2);
	snode.sign1 = sign1;
	snode.sign2 = sign2;

	if (ODB_SEEK_OK == odb_seek(sdb, &snode)) {
		printf("seek a node from dictionary OK\n");
		printf("sign1: %u, sign2: %u, value1: %u, vlaue2: %u\n", snode.sign1, snode.sign2, 
			snode.cuint1, snode.cuint2);
	}
	else
		printf("seek a node from dictionary FAIL\n");

	return 0;
}

int build_action_save(sodict_build_t *sdb)
{
	char		   name[128];
 
	printf("please enter dictionary name: ");
	fscanf(stdin, "%s", name);

	printf("save dictionary object to file ./%s ... \t", name);
	if (ODB_SAVE_OK == odb_save(sdb, "./", name)) {
		printf("OK\n");
	}
	else
		printf("FAIL\n");

	return 0;
}

int build_action_minsert(sodict_build_t *sdb)
{
	sodict_snode_t snode;
	int			ret;

	printf("insert 100 nodes to dictionary object ...\t");

	for (int i = 0 ; i < 100 ; i++) {
		snode.sign1 = 23 + i;
		snode.sign1 = 10384 + i;
		snode.cuint1 = i;
		snode.cuint2 = i;
		ret = odb_add(sdb, &snode, 1);
		if ((ODB_ADD_OK != ret) && (ODB_ADD_OVERWRITE != ret)) {
			printf("FAIL\n");
		}
	}
	printf("OK\n");

	return 0;
}

int build_action(sodict_build_t *sdb)
{
	char type[10];

	while(1) {
		printf("\n================== build test of odict ================\n");
		printf("1. insert a node to dictionary, please select 1 or i\n");
		printf("2. delete a node from the dictionary, please select 2 or d\n");
		printf("3. modify a node in the dictionary, please select 3 or m\n");
		printf("4. search a node in the dictionary, please select 4 or f\n");
		printf("5. list all the nodes in the dictionary, please select 5 or l\n");
		printf("6. save the dictionary object to files, please select 6 or s\n");
		printf("7. insert 100 nodes into dictionary, please select 7 or I\n");
		printf("8. load into dictionary from ./abcdef.*, please select 8 or L\n");
		printf("9. save dictionary object to file ./abcdef.*, please select 9 or S\n");
		printf("A. save dictionary object to file ./a/abcdef.*, please select A\n");
		printf("0. exit this menu, please select 0 or q\n");
		printf("Your choice: ");
		fscanf(stdin, "%s", type);
		switch(type[0]) {
			case 'i':
			case '1':   build_action_add(sdb);
						break;
			case 'd':
			case '2':   build_action_del(sdb);
						break;
			case 'm':
			case '3':   build_action_mod(sdb);
						break;
			case 'f':
			case '4':   build_action_seek(sdb);
						break;
			case 'l':
			case '5':   odb_build_report(sdb);
						break;
			case 's':
			case '6':   build_action_save(sdb);
						break;
			case 'I':
			case '7':   build_action_minsert(sdb);
						break;
			case 'L':
			case '8':   
						odb_destroy(sdb);
						sdb = odb_load("./", "abcdef", 0);
						if (sdb == NULL || sdb == (sodict_build_t*)ODB_LOAD_NOT_EXISTS) {
							printf("Load dictionary object ... FAIL\n");
						}
						else {
							printf("Load dictionary object ... OK\n");
						}
						break;
			case 'S':
			case '9':   if (ODB_SAVE_OK == odb_save_safely(sdb, "./", "abcdef")) {
							printf("Save dictionary object ... OK\n");
						}
						else {
							printf("Save dictionary object ... FAIL\n");
						}
						break;
			case 'A':   if (ODB_SAVE_OK == odb_save(sdb, "./a/", "abcdef")) {
							printf("Save dictionary object ... OK\n");
						}
						else {
							printf("Save dictionary object ... FAIL\n");
						}
						break;
			case 'q':
			case '0':   return 0;
		}
	}

	return 0;
}

int search_action_seek(sodict_search_t *sdb)
{
	char		   name[128];
	u_int		  sign1, sign2;
	sodict_snode_t snode;
 
	printf("please enter name: ");
	fscanf(stdin, "%s", name);

	creat_sign_fs64(name, strlen(name), &sign1, &sign2);
	snode.sign1 = sign1;
	snode.sign2 = sign2;

	if (ODB_SEEK_OK == odb_seek_search(sdb, &snode)) {
		printf("seek a node from dictionary OK\n");
		printf("sign1: %u, sign2: %u, value1: %u, vlaue2: %u\n", snode.sign1, snode.sign2, 
			snode.cuint1, snode.cuint2);
	}
	else
		printf("seek a node from dictionary FAIL\n");

	return 0;
}

int search_action(sodict_search_t *sdb)
{
	char type[10];

	while(1) {
		printf("\n================== search test of odict ================\n");
		printf("1. search a node in the dictionary, please select 1 or s\n");
		printf("2. report all nodes in the dictionary, please select 2 or r\n");
		printf("3. exit this menu, please select 3 or q\n");
		printf("Your choice: ");
		fscanf(stdin, "%s", type);
		switch(type[0]) {
			case 's':
			case '1':   search_action_seek(sdb);
						break;
			case 'r':
			case '2':   odb_search_report(sdb);
						break;
			case 'q':
			case '3':   return 0;
		}
	}

	return 0;
}


int main(int argc, char *argv[])
{
	sodict_build_t *sdb;
	sodict_search_t *ssdb;

 

	printf("Give an example for odict usage!\n");

	printf("Create a odict dictionary object ... \t");
	sdb = odb_creat(3);
	if (sdb != NULL) {
		printf("OK\n");
	}
	else {
		printf("FAIL\n");
		return 1;
	}



	build_action(sdb);

	printf("Now save the dictionary to file test_dict safely ...\t");
	if (ODB_SAVE_OK == odb_save_safely(sdb, "./", "test_dict"))
		printf("OK\n");
	else
		printf("FAIL\n");

	printf("Now destroy the dictionary object!\n");
	odb_destroy(sdb);

	
	printf("Now reload the dictionary for searching test\n");
	ssdb = odb_load_search("./", "test_dict");
	search_action(ssdb);

	printf("Now destroy the dictionary object!\n");
	odb_destroy_search(ssdb);

  

	
	return 0;
}
