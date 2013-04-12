#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "matching_test.h"

int main(int args, char** argv){

	int return_code;
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_Loop_matching algorithm", set_up, tear_down);

	if (NULL == pSuite){
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	/* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
	if ((NULL == CU_add_test(pSuite, "test install empty flowmod", test_install_empty_flow_mod)) ||
	(NULL == CU_add_test(pSuite, "test uninstall all", test_install_overlapping_specific)) ||
	(NULL == CU_add_test(pSuite, "test uninstall wildcard", test_uninstall_wildcard)) || 
	(NULL == CU_add_test(pSuite, "test check overlap addition", test_overlap)) || 
	(NULL == CU_add_test(pSuite, "test flow modify", test_flow_modify)) 
	
		)
	{
		return_code = CU_get_error();
		CU_cleanup_registry();
		return return_code;
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	return_code = CU_get_number_of_failures();
	CU_cleanup_registry();

	return return_code;
}