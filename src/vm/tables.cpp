#include "structs.h"
#include "mem.h"
#include <assert.h>

#define HASH_MULTIPLIER 65599

Table::Table() {
    this->ref_counter = 0;
    this->total = 0;
    this->addr = (char*)this;

    initBuckets(str_indexed);
    initBuckets(num_indexed);
    initBuckets(tab_indexed);
}

void Table::initBuckets(bucket **t) {
    for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        t[i] = (bucket *)0;
    }
}

void Table::destroyBuckets(bucket **t) {
    for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; i++, ++t) {
        for (bucket *b = *t; b;) {
            bucket *del = b;
            b = b->next;
            memclear::clcell(&del->key);
            memclear::clcell(&del->value);
            free(del);
        }
        t[i] = (bucket *)0;
    }
}

void Table::incRefCounter() {
    ++this->ref_counter;
}

void Table::decRefCounter() {

}

void Table::destroy() {
    this->destroyBuckets(this->str_indexed);
    this->destroyBuckets(this->num_indexed);
}

unsigned Table::hash(AVM_memcell *key) {
    size_t i;
    unsigned int sum = 0;
    char *str;

    using namespace AVM_memcell_t;
    switch (key->type) {
    case number_m:

        sum = key->data.numVal * HASH_MULTIPLIER;

        break;
    case string_m:
        str = key->data.strVal;

        for (i = 0; i < strlen(str); i++) {
            sum = sum * HASH_MULTIPLIER + str[i];
        }
        break;
    case table_m:
        sum = (unsigned int)key->data.tableVal->addr * AVM_TABLE_HASHSIZE;
        break;
    case bool_m:
        sum = key->data.boolVar * HASH_MULTIPLIER;
        break;
    case libfunc_m:
        str = key->data.libfuncVal;

        for (i = 0; i < strlen(str); i++) {
            sum = sum * HASH_MULTIPLIER + str[i];
        }
        break;
    case userfunc_m:
        sum = key->data.funcVal * HASH_MULTIPLIER;
        break;
    default:
        assert(0);
    }


    return sum % AVM_TABLE_HASHSIZE;
}

bucket **Table::getTable(AVM_memcell *key) {
    using namespace AVM_memcell_t;

    switch (key->type) {
    case number_m:
        return this->num_indexed;
        break;
    case string_m:
        return this->str_indexed;
        break;
    case table_m:
        return this->tab_indexed;
        break;
    case bool_m:
        return this->bol_indexed;
        break;
    case userfunc_m:
        return this->usr_indexed;
        break;
    case libfunc_m:
        return this->lib_indexed;
        break;
    default:
        assert(0);
    }
    return (bucket **)0;
}

bool keycmp(AVM_memcell *key1, AVM_memcell *key2) {
    using namespace AVM_memcell_t;

    switch (key1->type) {
    case number_m:
        if (key1->data.numVal == key2->data.numVal)
            return true;
        else
            return false;
        break;
    case string_m:
        if (strcmp(key1->data.strVal, key2->data.strVal) == 0)
            return true;
        else
            return false;
        break;
    case table_m:
        if (key1->data.tableVal->addr == key2->data.tableVal->addr)
            return true;
        else
            return false;
        break;
    case bool_m:
        if (key1->data.boolVar == key2->data.boolVar)
            return true;
        else
            return false;
        break;
    case libfunc_m:
        if (strcmp(key1->data.libfuncVal, key2->data.libfuncVal) == 0)
            return true;
        else
            return false;
        break;
    case userfunc_m:
        if (key1->data.funcVal == key2->data.funcVal)
            return true;
        else
            return false;
        break;
    default:
        assert(0);
    }
    return false;
}

AVM_memcell *Table::getElem(AVM_memcell *key) {
    bucket **table;
    int h;

    table = this->getTable(key);

    h = this->hash(key);

    bucket *iter = table[h];

    while (iter) {

        if (keycmp(&iter->key, key)) {
            return &iter->value;
        }

        iter = iter->next;
    }

    return 0;
}

void Table::setElem(AVM_memcell *key, AVM_memcell *value) {
    bucket **table;
    bucket *newnode;
    int h;

    table = this->getTable(key);

    h = this->hash(key);

    bucket *iter = table[h];

    if (!iter) {
        table[h] = (bucket *)malloc(sizeof(bucket));
        memcpy(&table[h]->key, key, sizeof(AVM_memcell));
        memcpy(&table[h]->value, value, sizeof(AVM_memcell));
        table[h]->next = (bucket *)0;
        this->total++;
        return;
    }


    bucket *prev = iter;
    while (iter) {
        if (keycmp(&iter->key, key)) {
            memcpy(&iter->key, key, sizeof(AVM_memcell));
            memcpy(&iter->value, value, sizeof(AVM_memcell));
            return;
        }

        prev = iter;
        iter = iter->next;
    }

    prev->next = (bucket *)malloc(sizeof(bucket));
    prev->next->key = *key;
    prev->next->value = *value;
    prev->next->next = (bucket *)0;
    this->total++;

    return;
}