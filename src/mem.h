typedef struct {
  uint32_t type;
  uint32_t dim;
  uint32_t *shape;
  uint32_t size;
  uint8_t *data;
} mrb_mem;

enum mem_type {
  mem_type_uint8 = 0,
  mem_type_int8,
  mem_type_uint16,
  mem_type_int16_t,
  mem_type_uint32_t,
  mem_type_int32_t,
  mem_type_uint64_t,
  mem_type_int64_t,
  mem_type_float,
  mem_type_double
};

uint8_t mem_type_size[] = {
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