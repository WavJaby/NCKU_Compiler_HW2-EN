//
// Created by WavJaby on 2026/3/2.
//

#include "value_data.h"

#include <string.h>

#include "compiler_util.h"

// linkedList_init / linkedList_addp / linkedList_deleteNode / linkedList_freeA / cloneStruct usage: see README.md §Utility Function Reference
bool object_ValueDataListCreate(ObjectType valueType, const ScientificNotation* count, ValueData* valueData) {
    linkedList_init(&valueData->valueList);
    valueData->valueType = valueType;
    valueData->count = (count != NULL) ? sciToInt32(count) : 1;
    // TODO: validate count > 0; call yyerrorf and return true for non-positive values
    return false;
}

bool object_ValueDataListAdd(ValueData* valueData, const Object* obj, const YYLTYPE* tokenLoc) {
    Object* clone = cloneStruct(Object, obj);
    // TODO: type compatibility check (compare objValueType against valueData->valueType)
    //       AUTO type should be resolved here; exceeding count limit should report an error
    if (obj->type == OBJECT_TYPE_STR && obj->value.str)
        clone->value.str = strdup(obj->value.str);
    linkedList_addp(&valueData->valueList, 0, clone); // freeFlag=0: no auto-free; freed uniformly by freeA(free)
    return false;
}

bool object_ValueDataListAddDefaults(ValueData* valueData, const YYLTYPE* tokenLoc) {
    // TODO: based on valueData->valueType, fill remaining slots (count - current element count) with zero values for each type
    //       use the appropriate object_create* to create values, then add them via object_ValueDataListAdd
    return false;
}

Object* object_ValueDataListPop(ValueData* valueData) {
    if (valueData->valueList.length == 0)
        return NULL;
    LinkedListNode* node = valueData->valueList.head->next;
    Object* obj = node->value;
    linkedList_deleteNode(&valueData->valueList, node);
    return obj;
}

bool object_ValueDataListFree(ValueData* valueData) {
    linkedList_freeA(&valueData->valueList, free);
    return false;
}
