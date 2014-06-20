#include "range.h"
#include <string.h>
#include <stdlib.h>

Range :: Range(const Address &ul,const Address &lr)
{
    count = 1;
    _ul = (Address *)malloc(sizeof(Address) * count);
    _lr = (Address *)malloc(sizeof(Address) * count);

    _ul[0] = ul;
    _lr[0] = lr;
}


Range :: Range(const Range &rng)
{
    count = rng.count;
    _ul = (Address *)malloc(sizeof(Address) * count);
    _lr = (Address *)malloc(sizeof(Address) * count);

    for (int x = 0; x < count ; x++) {
       _ul[x] = rng._ul[x];
       _lr[x] = rng._lr[x];
    } /* endfor */
}

Range :: ~Range()
{
    if (count) {
        free(_ul);
        free(_lr);
    } /* endif */
}

void Range :: getItem(int x, Address &ul, Address & lr) const
{
    if (x >= count) {
        ul.makeNull();
        lr.makeNull();
    } else {
        ul = _ul[x];
        lr = _lr[x];
    } /* endif */
}

void Range :: setItem(int x, Address &ul, Address & lr)
{
    if (x >= count) {
        add(ul,lr);
    } else {
        ul = _ul[x];
        lr = _lr[x];
    } /* endif */
    
}

void Range :: add(Address &ul, Address & lr)
{
    count++;
    _ul = (Address *)realloc(_ul,sizeof(Address) * count);
    _lr = (Address *)realloc(_lr,sizeof(Address) * count);

    _ul[count-1] = ul;
    _lr[count-1] = lr;
}
    
Range & Range :: operator = (const Range & rg)
{
    count = rg.count;
    _ul = (Address *)malloc(sizeof(Address) * count);
    _lr = (Address *)malloc(sizeof(Address) * count);

    for (int x = 0; x < count ; x++) {
       _ul[x] = rg._ul[x];
       _lr[x] = rg._lr[x];
    } /* endfor */   

    return *this;
}
    
void Range :: zap()
{
    if (count) {
        free(_ul);
        free(_lr);
        count = 0;
        _ul = NULL;
        _lr = NULL;
    } /* endif */
}

