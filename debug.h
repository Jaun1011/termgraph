
//
// \e]8;;
#define debug_string(val)               printf("%s:%i\t%p\t%s\n", __FILE__, __LINE__ , val, val)
#define debug_int(val)                  printf("%s:%i\t%p\tint %s\t%i\n", __FILE__, __LINE__ , val, #val, val)
#define debug_int_p(val)                printf("%s:%i\t%i\n", __FILE__, __LINE__ , val)
#define debug_int_msg(msg, val)         printf("%s:%i\t%s\t%p\tint %s\t%i\n", __FILE__, __LINE__, msg , val,  #val, val)
#define debug_float(val)                printf("%s:%i\t%p\tfloat %s\t%f\n", __FILE__, __LINE__ , val, #val, val)
#define debug_double(val)               printf("%s:%i\t%p\tdouble %s\t%d\n", __FILE__, __LINE__ , val, #val, val)
#define debug_pointer(val)              printf("%s:%i\t%p\t%s\t%p\n", __FILE__, __LINE__ , &val, #val, val)
#define print_splitter()                printf("-------------------------------\n")


