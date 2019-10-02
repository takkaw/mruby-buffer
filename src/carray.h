typedef struct {
  uint32_t type;
  uint32_t dim;
  uint32_t *shape;
  uint32_t size;
  uint8_t *data;
} mrb_carray;

enum carray_type {
  ca_type_none = -1,
  ca_type_uint8,
  ca_type_int8,
  ca_type_uint16,
  ca_type_int16_t,
  ca_type_uint32_t,
  ca_type_int32_t,
  ca_type_uint64_t,
  ca_type_int64_t,
  ca_type_float,
  ca_type_double
};

uint8_t carray_type_size[] = {
  sizeof(uint8_t),
  sizeof(int8_t),
  sizeof(uint16_t),
  sizeof(int16_t),
  sizeof(uint32_t),
  sizeof(int32_t),
  sizeof(uint64_t),
  sizeof(int64_t),
  sizeof(float),
  sizeof(double)
};
