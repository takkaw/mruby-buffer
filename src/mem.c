#include <mruby.h>
#include <mruby/data.h>
#include <mruby/class.h>
#include <mruby/variable.h>
#include <mruby/hash.h>
#include <mruby/array.h>
#include <mruby/string.h>

#include "mem.h"
#include "string.h"

static void mrb_mem_free(mrb_state *mrb, void *ptr)
{
  mrb_mem *mem = ptr;
  mrb_free(mrb, mem->shape);
  mrb_free(mrb, mem->data);
  mrb_free(mrb, mem);
}

static const struct mrb_data_type mrb_mem_type = {
  "Mem", mrb_mem_free
};

MRB_API mrb_value
mrb_mem_support_float(mrb_state *mrb, mrb_value self)
{
#ifdef MRB_WITHOUT_FLOAT
  return mrb_false_value();
#else
  return mrb_true_value();
#endif
}

MRB_API mrb_value
mrb_mem_alloc(mrb_state *mrb, mrb_value self)
{
  mrb_mem *mem;
  mrb_value *mrb_shape;
  mrb_value mrb_type_no;
  uint32_t *shape_ptr;
  mrb_int i;

  mem = mrb_malloc(mrb, sizeof(mrb_mem));
  mem->shape = NULL;
  mem->data = NULL;

  DATA_TYPE(self) = &mrb_mem_type;
  DATA_PTR(self) = mem;

  mrb_get_args(mrb, "oa", &mrb_type_no, &mrb_shape, &mem->dim);

  if( mrb_fixnum_p(mrb_type_no) ){
    mem->type = mrb_fixnum(mrb_type_no);
  }
  else{
  }

  mem->shape = mrb_malloc(mrb, sizeof(uint32_t) * mem->dim);
  mem->size = 1;
  shape_ptr = mem->shape;
  
  for( i = 0; i < mem->dim; i++ ) {
    if( mrb_fixnum_p(*mrb_shape) ) {
      *shape_ptr = mrb_fixnum(*mrb_shape);
    }
    mem->size *= *shape_ptr;
    shape_ptr++;
    mrb_shape++;
  }

  mem->data = mrb_malloc(mrb, mem->size * mem_type_size[mem->type]);

#if 0
  for( i = 0; i < (length * channel); i++ ){
    ab->data[i] = 0;
  }
#endif

  return self;
}

MRB_API mrb_value
mrb_mem_init_copy(mrb_state *mrb, mrb_value copy)
{
  mrb_mem *mem;
  mrb_value src;
  uint32_t *src_shape_ptr, *dst_shape_ptr;
  uint8_t *src_data_ptr;
  uint8_t *dst_data_ptr;
  mrb_int i;

  mrb_get_args(mrb, "o", &src);

  if(mrb_obj_equal(mrb, copy, src)) return copy;

  if(!mrb_obj_is_instance_of(mrb, src, mrb_obj_class(mrb, copy))) {
    mrb_raise(mrb, E_TYPE_ERROR, "wrong argument class");
  }

  if(!DATA_PTR(copy)) {
    mem = mrb_malloc(mrb, sizeof(mrb_mem));
    mem->shape = NULL;
    mem->data = NULL;

    DATA_TYPE(copy) = &mrb_mem_type;
    DATA_PTR(copy) = mem;

    mem->type = ((mrb_mem*)DATA_PTR(src))->type;
    mem->dim = ((mrb_mem*)DATA_PTR(src))->dim;

    mem->shape = mrb_malloc(mrb, sizeof(uint32_t) * mem->dim);
    
    src_shape_ptr = ((mrb_mem*)DATA_PTR(src))->shape;
    dst_shape_ptr = mem->shape;
    for( i = 0; i < mem->dim; i++ ) {
      *dst_shape_ptr++ = *src_shape_ptr++;
    }

    mem->size = ((mrb_mem*)DATA_PTR(src))->size;
    mem->data = mrb_malloc(mrb, mem->size * mem_type_size[mem->type]);

    src_data_ptr = ((mrb_mem *)DATA_PTR(src))->data;
    dst_data_ptr = mem->data;

    for( i = 0; i < (mem->size * mem_type_size[mem->type]); i++ ){
      *dst_data_ptr++ = *src_data_ptr++;
    }
  }

  return copy;
}

MRB_API mrb_value
mrb_mem_get_type_no(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(((mrb_mem *)DATA_PTR(self))->type);
}

MRB_API mrb_value
mrb_mem_get_dim(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(((mrb_mem *)DATA_PTR(self))->dim);
}

MRB_API mrb_value
mrb_mem_get_size(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(((mrb_mem *)DATA_PTR(self))->size);
}

MRB_API mrb_value
mrb_mem_get_shape(mrb_state *mrb, mrb_value self)
{
  mrb_value ary;
  uint32_t dim = ((mrb_mem *)DATA_PTR(self))->dim;
  uint32_t *shape = ((mrb_mem *)DATA_PTR(self))->shape; 
  uint32_t i;

  ary = mrb_ary_new_capa(mrb, dim);

  for(i = 0; i < dim; i++) {
    mrb_ary_set(mrb, ary, i, mrb_fixnum_value(*shape++));
  }
  return ary;
}

static uint8_t* mrb_mem_at(mrb_state *mrb, mrb_value self, mrb_value *mrb_elm, uint32_t elm_num)
{
  uint32_t dim, size, type;
  uint32_t *shape;
  uint32_t pos = 0;
  mrb_mem *mem= DATA_PTR(self);

  if( elm_num != mem->dim ){
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid dim number");
  }

  shape = mem->shape;
  size = mem->size;

  for( dim = 0; dim < (mem->dim); dim++ ){
    size /= *(shape++);

    if(mrb_fixnum_p(*mrb_elm)) {
      if( mrb_fixnum(*mrb_elm) < (mem->shape[dim]) ) {
        pos += mrb_fixnum(*mrb_elm) * size;
      }
      else {
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "dim[%d] size %d is must under %d", dim, mrb_fixnum(*mrb_elm), mem->shape[dim]);
      }
    }
    else{
      mrb_raise(mrb, E_ARGUMENT_ERROR, "shape must be fixnum");
    }
    mrb_elm++;
  }

  type = ((mrb_mem *)DATA_PTR(self))->type;

  pos *= mem_type_size[type];
  return ((mrb_mem *)DATA_PTR(self))->data + pos;
}

MRB_API mrb_value
mrb_mem_get_value(mrb_state *mrb, mrb_value self)
{
  mrb_value *mrb_elm;
  uint32_t type,elm_num;
  uint8_t *data;

  mrb_get_args(mrb, "*", &mrb_elm, &elm_num);

  type = ((mrb_mem *)DATA_PTR(self))->type;
  data = mrb_mem_at(mrb, self, mrb_elm, elm_num);

  switch(type) {
    case mem_type_uint8:    return mrb_fixnum_value( *(uint8_t *)data );
    case mem_type_int8:     return mrb_fixnum_value( *(int8_t *)data );
    case mem_type_uint16:   return mrb_fixnum_value( *(uint16_t *)data );
    case mem_type_int16_t:  return mrb_fixnum_value( *(int16_t *)data );
    case mem_type_uint32_t: return mrb_fixnum_value( *(uint32_t *)data );
    case mem_type_int32_t:  return mrb_fixnum_value( *(int32_t *)data );
    case mem_type_uint64_t: return mrb_fixnum_value( *(uint64_t *)data );
    case mem_type_int64_t:  return mrb_fixnum_value( *(int64_t *)data );
#ifndef MRB_WITHOUT_FLOAT
    case mem_type_float:    return mrb_float_value( mrb, *(float *)data );
    case mem_type_double:   return mrb_float_value( mrb, *(double *)data );
#endif
    default:                mrb_raisef(mrb, E_ARGUMENT_ERROR, "unexpected type %d at get", type);
  }
}

#ifndef MRB_WITHOUT_FLOAT
#define mrb_mem_set_val(data,type,val) \
do { \
  if( mrb_fixnum_p(val) ) { \
    *(type *)data = mrb_fixnum(val); \
  } \
  else if( mrb_float_p(val) ){ \
    *(type *)data = mrb_float(val); \
  } \
} while(0)
#else
#define mrb_mem_set_val(data,type,val) \
do { \
  if( mrb_fixnum_p(val) ) { \
    *(type *)data = mrb_fixnum(val); \
  } \
} while(0)
#endif

static void mrb_mem_set_val_each_type(mrb_state *mrb, uint8_t *data, uint32_t type, mrb_value val)
{
  switch(type) {
    case mem_type_uint8:    mrb_mem_set_val(data, uint8_t,  val); break;
    case mem_type_int8:     mrb_mem_set_val(data, int8_t,   val); break;
    case mem_type_uint16:   mrb_mem_set_val(data, uint16_t, val); break;
    case mem_type_int16_t:  mrb_mem_set_val(data, int16_t,  val); break;
    case mem_type_uint32_t: mrb_mem_set_val(data, uint32_t, val); break;
    case mem_type_int32_t:  mrb_mem_set_val(data, int32_t,  val); break;
    case mem_type_uint64_t: mrb_mem_set_val(data, uint64_t, val); break;
    case mem_type_int64_t:  mrb_mem_set_val(data, int64_t,  val); break;
#ifndef MRB_WITHOUT_FLOAT
    case mem_type_float:    mrb_mem_set_val(data, float,    val); break;
    case mem_type_double:   mrb_mem_set_val(data, double,   val); break;
#endif
    default:                mrb_raisef(mrb, E_ARGUMENT_ERROR, "unexpected type %d at set", type);
  }
}

MRB_API mrb_value
mrb_mem_set_value(mrb_state *mrb, mrb_value self)
{
  mrb_value *mrb_elm;
  uint32_t type,elm_num;
  uint8_t *data;

  mrb_get_args(mrb, "*", &mrb_elm, &elm_num);

  type = ((mrb_mem *)DATA_PTR(self))->type;
  data = mrb_mem_at(mrb, self, mrb_elm, elm_num-1);
  mrb_elm = mrb_elm + elm_num - 1;

#ifndef MRB_WITHOUT_FLOAT
  if( !mrb_fixnum_p(*mrb_elm) && !mrb_float_p(*mrb_elm) ) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "argument must be Fixnum or Float");
  }
#else
  if(!mrb_fixnum_p(*mrb_elm)) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "argument aust be Fixnum");
  }
#endif

  mrb_mem_set_val_each_type(mrb, data, type, *mrb_elm);

  return *mrb_elm; 
}

MRB_API mrb_value
mrb_mem_fill_value(mrb_state *mrb, mrb_value self)
{
  mrb_value mrb_val;
  uint32_t type,size,i;
  uint8_t *data;

  mrb_get_args(mrb, "o", &mrb_val);

  data = ((mrb_mem *)DATA_PTR(self))->data;
  type = ((mrb_mem *)DATA_PTR(self))->type;
  size = ((mrb_mem *)DATA_PTR(self))->size;
  
  for(i = 0; i < size; i++){
    mrb_mem_set_val_each_type(mrb, data, type, mrb_val);
    data += mem_type_size[type];
  }

  return mrb_val; 
}

void mrb_mruby_mem_gem_init(mrb_state *mrb)
{
  struct RClass *cls = mrb_define_class(mrb, "Mem", mrb->object_class);

  mrb_define_class_method(mrb, cls, "support_float?", mrb_mem_support_float, MRB_ARGS_NONE());

  MRB_SET_INSTANCE_TT(cls, MRB_TT_DATA);

  mrb_define_method(mrb, cls, "alloc", mrb_mem_alloc, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, cls, "initialize_copy", mrb_mem_init_copy, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, cls, "type_no", mrb_mem_get_type_no, MRB_ARGS_NONE());
  mrb_define_method(mrb, cls, "dim", mrb_mem_get_dim, MRB_ARGS_NONE());
  mrb_define_method(mrb, cls, "size", mrb_mem_get_size, MRB_ARGS_NONE());
  mrb_define_method(mrb, cls, "shape", mrb_mem_get_shape, MRB_ARGS_NONE());

  mrb_define_method(mrb, cls, "fill", mrb_mem_fill_value, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, cls, "[]", mrb_mem_get_value, MRB_ARGS_ANY());
  mrb_define_method(mrb, cls, "[]=", mrb_mem_set_value, MRB_ARGS_ANY());
}

void mrb_mruby_mem_gem_final(mrb_state *mrb)
{
  // nothing to do.
}

