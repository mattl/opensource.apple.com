/* { dg-options "-m32 -I." } */


namespace ATL
{

typedef struct 
{
unsigned int size;
} my_module;

class CAM1;
__attribute__((weak)) CAM1* _p1 = __null;
class CAM1 : public my_module
{
};

template <class T>
class CAM1T : public CAM1
{
public :
	CAM1T() throw() {}
};

class CAM2;
__attribute__((weak)) CAM2* _pModule = __null;
class CAM2 : public CAM1T<CAM2>
{
public :
	CAM2() {}
};

}

