#include "tinytest.h"
#include "mavalloc.h"

// This test case will pass
int test_case_0()
{
    if( mavalloc_init( 2048, FIRST_FIT ) ) 
    {
        return 0;
    }

    mavalloc_destroy();

    return 1;
}

// This test case will fail
int test_case_1()
{
    char * ptr = ( char * ) mavalloc_alloc ( 65535 );

    if( ptr == NULL )
    {
        return 0;
    }

    mavalloc_free( ptr ); 

    return 1;
}

// Unit test for first fit
int test_case_2()
{
    if( mavalloc_init( 2048, FIRST_FIT ) ) 
    {
        return 0;
    }

    char * ptr1 = ( char * ) mavalloc_alloc( 2048 );

    if( ptr1 == NULL )
    {
        return 0;
    }

    mavalloc_free( ptr1 );

    char * ptr2 = ( char * ) mavalloc_alloc( 1024 );

    if( ptr2 == NULL )
    {
        return 0;
    }

    mavalloc_free( ptr2 );

    mavalloc_destroy( );

    return 1;
}

// Unit test for worst fit
int test_case_3()
{
    if( mavalloc_init( 2048, WORST_FIT ) ) 
    {
        return 0;
    }

    char * ptr1 = ( char * ) mavalloc_alloc( 2048 );

    if( ptr1 == NULL )
    {
        return 0;
    }

    mavalloc_free( ptr1 );

    char * ptr2 = ( char * ) mavalloc_alloc( 1024 );

    if( ptr2 == NULL )
    {
        return 0;
    }

    mavalloc_free( ptr2 );

    mavalloc_destroy( );

    return 1;
}

// Unit test for first fit
int first_fit_test()
{
    // Setup
    printf("\n Setting up first fit test: \n");

    mavalloc_init( 1840, FIRST_FIT );

    char * hole1 = (char *)mavalloc_alloc( 200 );
    char * block1 = (char *)mavalloc_alloc( 400 );
    char * hole2 = (char *)mavalloc_alloc( 240 );
    char * block2 = (char *)mavalloc_alloc( 400 );
    char * hole3 = (char *)mavalloc_alloc( 120 );
    char * block3 = (char *)mavalloc_alloc( 400 );
    char * hole4 = (char *)mavalloc_alloc( 80 );

    mavalloc_free( hole1 );
    mavalloc_free( hole2 );
    mavalloc_free( hole3 );
    mavalloc_free( hole4 );

    mavalloc_print();

    // First fit algorithm

    char * test1 = (char *)mavalloc_alloc( 80 );
    char * test2 = (char *)mavalloc_alloc( 200 );
    char * test3 = (char *)mavalloc_alloc( 60 );

    printf("\n Result of the first fit test: \n");
    mavalloc_print();

    mavalloc_destroy();

    return 1;
}

// Unit test for worst fit 
int worst_fit_test() 
{
    // Setup
    printf("\n Setting up worst fit test: \n");

    mavalloc_init( 1840, WORST_FIT );

    char * hole1 = (char *)mavalloc_alloc( 200 );
    char * block1 = (char *)mavalloc_alloc( 400 );
    char * hole2 = (char *)mavalloc_alloc( 240 );
    char * block2 = (char *)mavalloc_alloc( 400 );
    char * hole3 = (char *)mavalloc_alloc( 120 );
    char * block3 = (char *)mavalloc_alloc( 400 );
    char * hole4 = (char *)mavalloc_alloc( 80 );

    mavalloc_free( hole1 );
    mavalloc_free( hole2 );
    mavalloc_free( hole3 );
    mavalloc_free( hole4 );

    mavalloc_print();

    // Worst fit algorithm

    char * test1 = (char *)mavalloc_alloc( 80 );
    char * test2 = (char *)mavalloc_alloc( 200 );
    char * test3 = (char *)mavalloc_alloc( 60 );

    printf("\n Result of the worst fit test: \n");
    mavalloc_print();

    mavalloc_destroy();

    return 1;
}

// This test case did not allow the other test cases to run.
// I don't know why, so it's just commented out.
/*
int test_case_3()
{
  // intentionally fail this test for now
  
  TINYTEST_EQUAL(1, 0);

  return 1;
}
*/

int tinytest_setup(const char *pName)
{
    fprintf( stderr, "tinytest_setup(%s)\n", pName);
    return 0;
}



int tinytest_teardown(const char *pName)
{
    fprintf( stderr, "tinytest_teardown(%s)\n", pName);
    return 0;
}


TINYTEST_START_SUITE(MavAllocTestSuite);
  TINYTEST_ADD_TEST(test_case_0,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_1,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_2,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_3,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(worst_fit_test,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(first_fit_test,tinytest_setup,tinytest_teardown);
TINYTEST_END_SUITE();

TINYTEST_MAIN_SINGLE_SUITE(MavAllocTestSuite);
