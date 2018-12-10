
#include <limits>
#include <cfenv>
#include <cmath>
#include <utility>
#include <QtGlobal>
#ifdef __MSC_VER
#pragma  fenv_access ON
#endif

#include <botan/bigint.h>

#include <exception>
#include <QVariant>

struct TestOverflow
{
	inline static bool validPresisiNumber(double d)
	{
		switch (std::fpclassify(d)) {
		case FP_INFINITE:  return false;
		case FP_NAN:       return false; 
		default:           return true;
		}
	}
    inline static std::pair<bool,std::string> isoverflowDoubleValueHappen(){
        if(std::fetestexcept(FE_DIVBYZERO)){
            return std::make_pair(true,"error divide by zero ");
        }
        if(std::fetestexcept(FE_UNDERFLOW)){
            return std::make_pair(true,"error underflow exception");
        }
        if(std::fetestexcept(FE_INVALID)){
            return std::make_pair(true,"error invalid exception number");
        }
        if(std::fetestexcept(FE_OVERFLOW)){
            return std::make_pair(true,"error overflow exception");
        }
        return std::make_pair(false,"");
    }
    
    inline static qint64 toNumber(const Botan::BigInt &d){
        std::string stringnum;
        std::stringstream stm;
        stm<<d;
        stm>>stringnum;
        auto var=QByteArray::fromStdString (stringnum);
        return var.toLongLong ();
    }

inline static std::pair<bool,qint64> checkPlusOverflow(const qint64 &a,const qint64 &b)
{
QVariant v_a(a);
QVariant v_b(b);
QVariant v_min(std::numeric_limits<qint64>::min());
QVariant v_max(std::numeric_limits<qint64>::max());
Botan::BigInt aa(v_a.toString ().toStdString ()),bb(v_b.toString ().toStdString ()),minval(v_min.toString ()
                                                                                           .toStdString ()),maxval(v_max.toString ().toStdString ());
aa+=bb;
if (aa < minval || aa >maxval ){
return std::make_pair(true,0);
}
return std::make_pair(false,toNumber(aa));
}
inline static std::pair<bool, qint64> checkMinOverflow(const qint64 &a,const qint64 &b)
{
    QVariant v_a(a);
    QVariant v_b(b);
    QVariant v_min(std::numeric_limits<qint64>::min());
    QVariant v_max(std::numeric_limits<qint64>::max());
    Botan::BigInt aa(v_a.toString ().toStdString ()),bb(v_b.toString ().toStdString ()),minval(v_min.toString ()
                                                                                               .toStdString ()),maxval(v_max.toString ().toStdString ());
  aa -= bb;  
  if (aa < minval|| aa > maxval) {
	  return std::make_pair(true, 0);
  }
  return std::make_pair(false, toNumber(aa));

}
inline static std::pair<bool, qint64> checkMulOverflow(const qint64 &a,const qint64 &b)
{
    QVariant v_a(a);
    QVariant v_b(b);
    QVariant v_min(std::numeric_limits<qint64>::min());
    QVariant v_max(std::numeric_limits<qint64>::max());
    Botan::BigInt aa(v_a.toString ().toStdString ()),bb(v_b.toString ().toStdString ()),minval(v_min.toString ()
                                                                                               .toStdString ()),maxval(v_max.toString ().toStdString ());
	aa *= bb; 
	if (aa <minval|| aa > maxval) {
		return std::make_pair(true, 0);
	}
	return std::make_pair(false, toNumber(aa));
}
inline static std::pair<bool, qint64> checkDivOverflow(const qint64 &a,const qint64 &b)
{
    QVariant v_a(a);
    QVariant v_b(b);
    QVariant v_min(std::numeric_limits<qint64>::min());
    QVariant v_max(std::numeric_limits<qint64>::max());
    Botan::BigInt aa(v_a.toString ().toStdString ()),bb(v_b.toString ().toStdString ()),minval(v_min.toString ()
                                                                                               .toStdString ()),maxval(v_max.toString ().toStdString ());
	aa /= bb; 
	if (aa < minval || aa > maxval){
		return  std::make_pair(true, 0);
	}
	return std::make_pair(false, toNumber(aa));
}
inline static std::pair<bool,qint64> checkInRangeNumberFromStr(const std::string &s){
    QVariant v_min(std::numeric_limits<qint64>::min());
    QVariant v_max(std::numeric_limits<qint64>::max());
    Botan::BigInt aa(s),minval(v_min.toString ().toStdString ()),maxval(v_max.toString ().toStdString ());
    if(aa<minval){
        return std::make_pair(false,-1);
    }
    if(aa>maxval){
        return std::make_pair(false,1);
    }
    return std::make_pair(true,toNumber(aa));
}
};