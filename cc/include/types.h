#ifndef CC_INCLUDE_TYPES_H
#define CC_INCLUDE_TYPES_H

#include <stddef.h>

typedef struct Type Type;

typedef enum {
  INTEGRAL_CHAR,
  INTEGRAL_SHORT,
  INTEGRAL_INT,
  INTEGRAL_LONG
} PrimitiveTypeKind;

typedef struct {
  PrimitiveTypeKind kind;
  int is_signed;
} PrimitiveType;

typedef enum {
  FLOAT_SINGLE,
  FLOAT_DOUBLE,
  FLOAT_LONG_DOUBLE
} FloatingPointTypeKind;

typedef struct {
  FloatingPointTypeKind kind;
} FloatingPointType;

typedef struct {
  Type *pointee;
} PointerType;

typedef struct {
  Type *element_type;
  size_t size;
  int is_complete;
} ArrayType;

typedef struct {
  Type *return_type;
  Type **parameter_types;
  size_t parameter_count;
  int is_variadic;
} FunctionType;

typedef enum {
  RECORD_STRUCT,
  RECORD_UNION
} RecordTypeKind;

typedef struct {
  RecordTypeKind kind;
  char *name;
  char **field_names;
  Type **field_types;
  size_t field_count;
  int is_complete;
} RecordType;

typedef struct {
  char *name;
  char **enumerators;
  int *values;
  size_t enumerator_count;
  int is_complete;
} EnumType;

typedef enum {
  TYPE_VOID,
  TYPE_PRIMITIVE,
  TYPE_FLOATING_POINT,
  TYPE_POINTER,
  TYPE_ARRAY,
  TYPE_FUNCTION,
  TYPE_RECORD,
  TYPE_ENUM
} TypeKind;

struct Type {
  TypeKind kind;
  int is_const;
  int is_volatile;

  union {
    PrimitiveType primitive;
    FloatingPointType floating_point;
    PointerType pointer;
    ArrayType array;
    FunctionType function;
    RecordType record;
    EnumType enum_type;
  } as;
};

#endif
