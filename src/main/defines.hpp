#ifndef TRACTOR_CONVERTER_DEFINES_H
#define TRACTOR_CONVERTER_DEFINES_H



#define TRACTOR_CONVERTER_VERSION "1.0.0"

#define TRACTOR_CONVERTER_STR_HELPER(x) #x
#define TRACTOR_CONVERTER_STR(x) TRACTOR_CONVERTER_STR_HELPER(x)



// For program options.
#define TRACTOR_CONVERTER_DEFAULT_3D_OBJ_FLOAT_PRECISION 6
#define TRACTOR_CONVERTER_DEFAULT_3D_OBJ_FLOAT_PRECISION_STR \
  TRACTOR_CONVERTER_STR(TRACTOR_CONVERTER_DEFAULT_3D_OBJ_FLOAT_PRECISION)

#define TRACTOR_CONVERTER_DEFAULT_3D_DEFAULT_SCALE 0.195313
#define TRACTOR_CONVERTER_DEFAULT_3D_DEFAULT_SCALE_STR \
  TRACTOR_CONVERTER_STR(TRACTOR_CONVERTER_DEFAULT_3D_DEFAULT_SCALE)

#define TRACTOR_CONVERTER_DEFAULT_C3D_DEFAULT_MATERIAL "body_red"

#define TRACTOR_CONVERTER_DEFAULT_3D_SCALE_CAP 0.560547
#define TRACTOR_CONVERTER_DEFAULT_3D_SCALE_CAP_STR \
  TRACTOR_CONVERTER_STR(TRACTOR_CONVERTER_DEFAULT_3D_SCALE_CAP)

#define TRACTOR_CONVERTER_DEFAULT_MAX_SMOOTH_ANGLE "30d"



// For helpers.
enum class error_handling{none, throw_exception};



#endif // TRACTOR_CONVERTER_DEFINES_H

