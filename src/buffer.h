typedef struct {
  uint32_t dim;
  uint32_t *shape;
  uint32_t size;
  int8_t type;
  uint8_t *data;
} mrb_buffer;

static const char *buffer_sub_class_name_list[] = {
  "Uint8",
  "Int8",
  "Uint16",
  "Int16",
  "Uint32",
  "Int32",
  "Uint64",
  "Int64",
#ifndef MRB_WITHOUT_FLOAT
  "Float",
  "Double",
#endif
};

enum buffer_type {
  buffer_type_uint8 = 0,
  buffer_type_int8,
  buffer_type_uint16,
  buffer_type_int16_t,
  buffer_type_uint32_t,
  buffer_type_int32_t,
  buffer_type_uint64_t,
  buffer_type_int64_t,
#ifndef MRB_WITHOUT_FLOAT
  buffer_type_float,
  buffer_type_double,
#endif
};

uint8_t buffer_type_size[] = {
  sizeof(uint8_t),
  sizeof(int8_t),
  sizeof(uint16_t),
  sizeof(int16_t),
  sizeof(uint32_t),
  sizeof(int32_t),
  sizeof(uint64_t),
  sizeof(int64_t),
#ifndef MRB_WITHOUT_FLOAT
  sizeof(float),
  sizeof(double),
#endif
};

