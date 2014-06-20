// These define the basic objects used in the sheets.  
#ifndef __RANGE__
#define __RANGE__

#ifndef NULL
#define NULL 0
#endif 

// Addresses consist of a row, a column, and a layer.  This
// class is a very simplified class.  It doesn't do any bounds
// checking to make sure the values fall with Mesa's range.
class Address {
   public:
    Address() {r=c=l=-1;};
    Address(const Address & ad) {r=ad.r;c=ad.c;l=ad.l;};
    Address(int r1,int c1,int l1) { r=r1;c=c1;l=l1;};

    int row() const { return r;};
    int col() const { return c;};
    int layer() const {return l;};

    Address & set(int r1, int c1, int l1) { r=r1;c=c1;l=l1; return *this;};

    Address & setRow(int r1) {r=r1; return *this;};
    Address & setCol(int c1) {c=c1; return *this;};
    Address & setLayer(int l1) {l=l1;return *this;};

    void makeNull() { r = c = l = -1; };
    int isNull() { return ((r == -1) && (c == -1) && (l == -1));};

    int operator == (const Address & ad) const { return (r==ad.r && c==ad.c && l==ad.l);};
    int operator != (const Address & ad) const { return (r!=ad.r || c!=ad.c || l!=ad.l);};
    Address & operator = (const Address & ad) {r=ad.r;c=ad.c;l=ad.l; return *this;};

   private:
    int r,c,l;
};

// This is a simple range class.  It does handle discontinuous ranges.  
class Range {
   public:
    Range() { count = 0; _ul = NULL; _lr = NULL;};
    Range(const Address &ul,const Address &lr);
    Range(const Range &rng);
    ~Range();

    int getCount() const { return count;};
    
    void getItem(int x, Address &ul, Address & lr) const;
    void setItem(int x, Address &ul, Address & lr);

    void add(Address &ul, Address & lr);

    void zap();
    
    Range & operator = (const Range & rg);
    
   private:
    int count;
    Address *_ul;
    Address *_lr;
};

#endif
