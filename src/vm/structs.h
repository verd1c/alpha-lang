#ifndef __STRUCTS__
#define __STRUCTS__

#define AVM_STACKSIZE   4096

#define AVM_TABLE_HASHSIZE	211



namespace AVM_memcell_t {
    enum AVM_memcell_t {
        number_m,
        string_m,
        bool_m,
        table_m,
        userfunc_m,
        libfunc_m,
        nil_m,
        undef_m
    };
}

union data_t {
    double numVal;
    char *strVal;
    unsigned char boolVar;
    struct AVM_table *tableVal;
    unsigned funcVal;
    char *libfuncVal;
};

struct AVM_memcell {
    AVM_memcell_t::AVM_memcell_t type;
    
    data_t data;
};
struct AVM_table_bucket {

	AVM_memcell 		key;
	AVM_memcell 		value;
	AVM_table_bucket*	next;

}; 

struct AVM_table {

	unsigned int 		ref_counter;
	AVM_table_bucket*	strIndexed[AVM_TABLE_HASHSIZE];
	AVM_table_bucket*	numIndexed[AVM_TABLE_HASHSIZE];
	unsigned int 		total;

};




class AVMStack {
public:
    AVM_memcell stack[AVM_STACKSIZE];
    AVMStack();
};

#endif