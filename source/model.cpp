#include <string.h>
#include <stdio.h>                 // added for printf

#include "model.h"
#include "extaddin.h"

Model :: Model()
{
    _model = (int)performAddInFunction(newModelMesaAddIn);
    _redisplay = 0;
}

Model ::Model(int m)
{
    switch (m) {
    case -1:
       _model = (int)performAddInFunction(getActiveModelMesaAddIn);
       break;
    case 0:
       _model = (int)performAddInFunction(newModelMesaAddIn);
       break;
    default:
       _model = m;
      break;
    } /* endswitch */
    _redisplay = 0;
}

Model :: Model(char *name)
{
    _model = (int)performAddInFunction(openModelMesaAddIn,name);
    _redisplay = 0;
}

Model & Model ::  getName(char * buf,int max)
{
    char * b2;
    b2 = (char *)performAddInFunction(getModelNameMesaAddIn,_model);
    if (!b2) {
       buf[0] = 0;
       return *this;
    } /* endif */
    if ((strlen(b2)+1) > max) {
        b2[max-1] = 0;
    } /* endif */
    strcpy(buf,b2);
    MFree(b2);
    return *this;
}

Model & Model :: setName(char *name)
{
    performAddInFunction(setModelNameMesaAddIn,_model,name,_redisplay);
    return *this;
}

Model & Model :: save()
{
    performAddInFunction(saveModelAddIn, _model);
    return *this;
}

Model & Model :: saveAs(char * name)
{
    setName(name);
    save();
    return *this;
}

int Model :: load(char * name)   // returns true if successfull
{
    _model = (int)performAddInFunction(openModelMesaAddIn,name);
    return _model != 0;
}

Model & Model :: addressToString(Address &a,char * buf,int max)
{
    char * b2;
    b2 = (char *)performAddInFunction(addressToStringMesaAddIn,_model,
             a.row(),a.col(),a.layer());

    if ((strlen(b2)+1) > max) {
        b2[max-1] = 0;
    } /* endif */
    strcpy(buf,b2);
    MFree(b2);
    return *this;
}

Address Model :: stringToAddress(char * buf)
{
    int r,c,l;
    int rc = (int) performAddInFunction(stringToAddressMesaAddIn, _model,buf,&r, &c, &l);
    if (rc) {
       return Address(r,c,l);
    }
    return Address(-1,-1,-1);
}

Model & Model :: selectNamedRange(char *buf)
{
    performAddInFunction(selectMesaAddIn, buf);
    return *this;
}

int Model :: setCellValue(const Address &ad,double db)
{
    MesaAddInValue aiv;
    int rc;

    performAddInFunction(initValueMesaAddIn, &aiv);
    aiv.row = ad.row();
    aiv.column = ad.col();
    aiv.layer = ad.layer();

    aiv.number = db;
    aiv.type = numberValueMesaAddInType;
    rc = (int) performAddInFunction(setCellValueMesaAddIn, _model,&aiv, _redisplay);
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: setCellValue(const Address &ad,char * str)
{
    MesaAddInValue aiv;
    int rc;

    performAddInFunction(initValueMesaAddIn, &aiv);
    aiv.row = ad.row();
    aiv.column = ad.col();
    aiv.layer = ad.layer();

    aiv.string = MMalloc(strlen(str)+1);
    strcpy(aiv.string,str);
    aiv.type = stringValueMesaAddInType;
    rc = (int) performAddInFunction(setCellValueMesaAddIn, _model,&aiv, _redisplay);
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: setCellError(const Address &ad,int error)
{
    MesaAddInValue aiv;
    int rc;

    performAddInFunction(initValueMesaAddIn, &aiv);
    aiv.row = ad.row();
    aiv.column = ad.col();
    aiv.layer = ad.layer();

    aiv.error = error;
    aiv.type = errorValueMesaAddInType;
    rc = (int) performAddInFunction(setCellValueMesaAddIn, _model,&aiv, _redisplay);
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: getCellValue(const Address &ad,double &db)
{
    MesaAddInValue aiv;
    int rc;

    performAddInFunction(initValueMesaAddIn, &aiv);
    aiv.row = ad.row();
    aiv.column = ad.col();
    aiv.layer = ad.layer();

    rc = (int) performAddInFunction(getCellValueMesaAddIn, _model,&aiv);
    if (aiv.type != numberValueMesaAddInType) {
       rc = 1;
    } /* endif */

    db = aiv.number;
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: getCellValue(const Address &ad,char * &str, int max)
{
    MesaAddInValue aiv;
    int rc;

    performAddInFunction(initValueMesaAddIn, &aiv);
    aiv.row = ad.row();
    aiv.column = ad.col();
    aiv.layer = ad.layer();

    rc = (int) performAddInFunction(getCellValueMesaAddIn, _model,&aiv);
    if (aiv.type != stringValueMesaAddInType) {
       rc = 1;
    } /* endif */

    if ((strlen(aiv.string)+1) > max) {
        aiv.string[max-1] = 0;
    } /* endif */
    strcpy(str,aiv.string);

    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: getCellError(const Address &ad,int &error)
{
    MesaAddInValue aiv;
    int rc;

    performAddInFunction(initValueMesaAddIn, &aiv);
    aiv.row = ad.row();
    aiv.column = ad.col();
    aiv.layer = ad.layer();

    rc = (int) performAddInFunction(getCellValueMesaAddIn, _model,&aiv);
    if (aiv.type != errorValueMesaAddInType) {
       rc = 1;
    } /* endif */

    error = aiv.error;
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: setCellString(const Address &ad,char * str)
{
    return (int)performAddInFunction(setCellStringMesaAddIn, _model,str,
                  ad.layer(),ad.row(), ad.col(),_redisplay);
}

Model & Model :: getCellString(const Address &ad,char * &str, int max)
{
   char * b2 = (char *)performAddInFunction(getCellOutputMesaAddIn,
           _model,ad.layer(), ad.row(),ad.col());
    if ((strlen(b2)+1) > max) {
        b2[max-1] = 0;
    } /* endif */
    strcpy(str,b2);
    MFree(b2);
    return *this;
}

Model & Model :: getCellOutput(const Address &ad,char * &str, int max)
{
   char * b2 = (char *)performAddInFunction(getCellStringMesaAddIn,
           _model,ad.layer(), ad.row(),ad.col());
    if ((strlen(b2)+1) > max) {
        b2[max-1] = 0;
    } /* endif */
    strcpy(str,b2);
    MFree(b2);
    return *this;
}

int Model :: setExtentsForLayer(int layer,int rows,int columns)
{
    return (int)performAddInFunction(setExtentsMesaAddIn, _model, layer,rows,
		columns, _redisplay);
}

int Model :: getExtentsForLayer(int layer,int &rows,int &columns)
{
    return (int)performAddInFunction(getExtentsMesaAddIn, _model,layer, &rows,
		&columns);
}

int Model :: getNumberOfLayers()
{
    return (int) performAddInFunction(getNumberOfLayersMesaAddIn,_model);
}

Model & Model :: addLayer()
{
    performAddInFunction(addALayerMesaAddIn,_model,_redisplay);
    return *this;
}

int Model :: setRowSize(int layer,int row, int size)
{
    return (int)performAddInFunction(setRowSizeMesaAddIn,_model,layer,row,
                    size,_redisplay);
}

int Model :: getRowSize(int layer,int row)
{
    return (int)performAddInFunction(getRowSizeMesaAddIn,_model,layer,row);
}

int Model :: setColSize(int layer,int col, int size)
{
    return (int)performAddInFunction(setColumnSizeMesaAddIn,_model,layer,col,
                    size,_redisplay);
}

int Model :: getColSize(int layer,int col)
{
    return (int)performAddInFunction(getColumnSizeMesaAddIn,_model,layer,col);
}


Model & Model :: recalc()
{
    performAddInFunction(recalcMesaAddIn, _model);
    return *this;
}
Model & Model :: redisplay()
{
    performAddInFunction(redisplayMesaAddIn, _model);
    return *this;
}

Model & Model :: setBlob(char * name, int len, void * data)
{
    if ((int)performAddInFunction(getVersionNumberMesaAddIn) > 102) {
        performAddInFunction(setBlobMesaAddIn,(void *)_model,(const char *)name,(int)len,(const void *)data);
    } // endif
    return *this;
}

int Model :: getBlob(char * name,int max, void * data)
{
    int len=0;
    void *dt=NULL;
    int rc = performAddInFunction(getBlobMesaAddIn,_model,name,&len,&dt);

    if (!rc) {   // blob wasn't found
        return 0;
    } /* endif */

    if (len <= max) {
       memcpy(data,dt,len);
       MFree(dt);
       return len;
    } else {
       memcpy(data,dt,max);
       MFree(dt);
       return max;
    } /* endif */
    MFree(dt);
    return 0;
}


Model & Model :: rangeToString(Range &r,char * buf, int max)
{
    char * b2;
    int x,y;
    Address a,b;
    int lensofar = 0;
    buf[0] = NULL;

    for (x = 0 ; x < r.getCount() ; x++) {
        r.getItem(x,a,b);
        b2 = (char *)performAddInFunction(rangeToStringMesaAddIn,_model,
             a.row(),a.col(),a.layer(),b.row(),b.col(),b.layer());

        if (lensofar) {                    // if it has a range, add a ,
            strcpy(&buf[lensofar],",");
            lensofar++;
        } /* endif */

        y = strlen(b2);
        if ((lensofar + y + 1) > max); {
           b2[max - lensofar - 1] = 0;
        } /* endif */

        strcpy(&buf[lensofar],b2);
        lensofar += strlen(b2);
        MFree(b2);
    } /* endfor */

    return *this;
}


Range Model :: stringToRange(char * buf)
{
    int r,c,l;
    int r2,c2,l2;
    int x;
    Range ra;
    Address a,b;
    int rc = (int) performAddInFunction(stringToRangeMesaAddIn, _model,buf,0,&r, &c, &l, &r2,&c2,&l2);

    for (x = 0; x < rc ; x++ ) {
        performAddInFunction(stringToRangeMesaAddIn, _model,buf,x,&r, &c, &l, &r2,&c2,&l2);
        a.set(r,c,l);
        b.set(r2,c2,l2);
        ra.add(a,b);
    } /* endfor */

    return ra;
}

int Model :: pushValue(void * stack,double db)
{
    MesaAddInValue aiv;
    int rc;

    performAddInFunction(initValueMesaAddIn, &aiv);

    aiv.number = db;
    aiv.type = numberValueMesaAddInType;
    rc = (int) performAddInFunction(pushValueMesaAddIn,stack, _model,&aiv);
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: pushValue(void * stack,char * str)
{
    MesaAddInValue aiv;
    int rc;

    performAddInFunction(initValueMesaAddIn, &aiv);

    aiv.string = MMalloc(strlen(str)+1);
    strcpy(aiv.string,str);
    aiv.type = stringValueMesaAddInType;
    rc = (int) performAddInFunction(pushValueMesaAddIn,stack, _model,&aiv);
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: pushError(void * stack,int error)
{
    MesaAddInValue aiv;
    int rc = 0;

    performAddInFunction(initValueMesaAddIn, &aiv);

    aiv.error = error;
    aiv.type = errorValueMesaAddInType;
    rc = (int) performAddInFunction(pushValueMesaAddIn,stack, _model,&aiv);
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: popValue(void * stack,double &db)
{
    MesaAddInValue aiv;
    int rc = 0;

    performAddInFunction(initValueMesaAddIn, &aiv);

    rc = (int) performAddInFunction(popElementMesaAddIn,stack, _model,&aiv);
    if (aiv.type != numberValueMesaAddInType) {
       rc = 1;
       performAddInFunction(pushValueMesaAddIn,stack, _model,&aiv);
    } /* endif */

    db = aiv.number;
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: popValue(void * stack,char * &str, int max)
{
    MesaAddInValue aiv;
    int rc = 0;

    performAddInFunction(initValueMesaAddIn, &aiv);

    rc = (int) performAddInFunction(popElementMesaAddIn,stack, _model,&aiv);
    if (aiv.type != stringValueMesaAddInType) {
       rc = 1;
       performAddInFunction(pushValueMesaAddIn,stack, _model,&aiv);
    } /* endif */

    if ((strlen(aiv.string)+1) > max) {
        aiv.string[max-1] = 0;
    } /* endif */
    strcpy(str,aiv.string);

    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

int Model :: popError(void * stack,int &error)
{
    MesaAddInValue aiv;
    int rc = 0;

    performAddInFunction(initValueMesaAddIn, &aiv);

    rc = (int) performAddInFunction(popElementMesaAddIn,stack, _model,&aiv);
    if (aiv.type != errorValueMesaAddInType) {
       rc = 1;
       performAddInFunction(pushValueMesaAddIn,stack, _model,&aiv);
    } /* endif */

    error = aiv.error;
    performAddInFunction(freeValueMesaAddIn, &aiv);
    return rc;
}

void Model :: initAddInValue(MesaAddInValue &aiv)
{
    performAddInFunction(initValueMesaAddIn, &aiv);
}

void Model :: freeAddInValue(MesaAddInValue &aiv)
{
    performAddInFunction(freeValueMesaAddIn, &aiv);
}

void Model :: pushValue( void * stack, MesaAddInValue &aiv)
{
    performAddInFunction(pushValueMesaAddIn,stack, _model,&aiv);
}

void Model :: popValue( void * stack, MesaAddInValue &aiv)
{
    performAddInFunction(popElementMesaAddIn,stack, _model,&aiv);
}

void Model :: getCellValue( Address & ad, MesaAddInValue &aiv)
{
    performAddInFunction(getCellValueMesaAddIn, _model,&aiv);
}

void Model :: setCellValue( Address & ad, MesaAddInValue &aiv)
{
    performAddInFunction(setCellValueMesaAddIn, _model,&aiv, _redisplay);
}

