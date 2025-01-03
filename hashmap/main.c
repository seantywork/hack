
#include <stdio.h>

#define SIZE_OF_MAP 1024


typedef struct Key {
    int x;
    int y;
    int i;
    int j;
}Key;

typedef struct Map {
    int assigned;
    Key key;
    int value;
}Map;



Map map[SIZE_OF_MAP];

int get_hash(Key *key)
{
    int result;
    /* combine all inputs in some way */
    result = key->i * key->i + (key->x * key->x) - (key->y * key->y);
    /* make sure result isn't out of bounds of the array */
    return (result % SIZE_OF_MAP);
}


int map_get(Key* key) {

    int val = 0;

    int hashval = get_hash(key);

    Map m = map[hashval];

    val = m.value;

    return val;
}


int map_set(Key* key, int value){

    int hashval = get_hash(key);

    Map m = {
        .assigned = 1,
        .key = *key,
        .value = value
    };


    map[hashval] = m;

    return hashval;

}

int main(){


    Key k1 = {
        .x = 1,
        .y = 2,
        .i = 3,
        .j = 4
    };

    int v1 = 1;


    Key k2 = {
        .x = 4,
        .y = 3,
        .i = 2,
        .j = 1
    };

    int v2 = 2;

    
    int result = map_set(&k1, v1);

    printf("set result: 1: %d\n", result);

    result = map_set(&k2, v2);

    printf("set result: 2: %d\n", result);

    result = map_get(&k1);

    printf("get result: 1: %d\n", result);

    result = map_get(&k2);

    printf("get result: 2: %d\n", result);

    return 0;
}