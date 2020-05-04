#include <mruby.h>
#include <mruby/data.h>
#include <mruby/class.h>
#include <mruby/variable.h>
#include <mruby/hash.h>
#include <mruby/array.h>
#include <mruby/string.h>

#include "buffer.h"
#include "string.h"

static struct RClass *buffer_sub_class_list[] = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
#ifndef MRB_WITHOUT_FLOAT
  NULL,
  NULL,
#endif
};

static void mrb_buffer_free(mrb_state *mrb, void *ptr)
{
  mrb_buffer *buffer = ptr;
  mrb_free(mrb, buffer->shape);
  mrb_free(mrb, buffer->data);
  mrb_free(mrb, buffer);
}

static const struct mrb_data_type mrb_buffer_type = {
  "Buffer", mrb_buffer_free
};

MRB_API mrb_value
mrb_buffer_support_float(mrb_state *mrb, mrb_value self)
{
#ifdef MRB_WITHOUT_FLOAT
  return mrb_false_value();
#else
  return mrb_true_value();
#endif
}

mrb_value
mrb_buffer_alloc(mrb_state *mrb, mrb_value self, mrb_int type_no, mrb_value mrb_shape, mrb_int dim)
{
  mrb_buffer *buffer;
  mrb_int i;

  buffer = mrb_malloc(mrb, sizeof(mrb_buffer));
  buffer->shape = NULL;
  buffer->data = NULL;
  buffer->dim = dim;
  buffer->type = type_no;

  DATA_TYPE(self) = &mrb_buffer_type;
  DATA_PTR(self) = buffer;

  buffer->shape = mrb_malloc(mrb, sizeof(uint32_t) * buffer->dim);
  buffer->size = 1;
  
  for( i = 0; i < buffer->dim; i++ ) {
    buffer->shape[i] = mrb_fixnum(mrb_ary_ref(mrb, mrb_shape, i));
    buffer->size *= buffer->shape[i];
  }

  buffer->data = mrb_malloc(mrb, buffer->size * buffer_type_size[buffer->type]);

  return self;
}

MRB_API mrb_value
mrb_buffer_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value mrb_arg, mrb_shape, shape_val;
  mrb_int shape_len, type_no, i;
  const char* class_name;

  class_name = mrb_obj_classname(mrb, self);

  mrb_int class_num = sizeof(buffer_sub_class_name_list) / sizeof(buffer_sub_class_name_list[0]);
  for( type_no = 0; type_no < class_num; type_no++) {
    if( mrb_obj_is_kind_of(mrb, self, buffer_sub_class_list[type_no]) ) {
      break;
    }
  }

  if( type_no >= class_num ) {
    mrb_raise(mrb, mrb_exc_get(mrb, "StandardError"), "Cannot create an instance, only can create Buffer sub class instance");
  }

  mrb_get_args(mrb, "o", &mrb_arg);

  if( mrb_fixnum_p(mrb_arg) ){
    shape_len = 1;
    mrb_shape = mrb_ary_new_capa(mrb, 1);
    mrb_ary_set(mrb, mrb_shape, 0, mrb_arg);
  }
  else if( mrb_array_p(mrb_arg) ) {
    mrb_shape = mrb_arg;
    shape_len = RARRAY_LEN(mrb_shape);
    for( i = 0; i < shape_len; i++) {
      shape_val = mrb_ary_ref(mrb, mrb_shape, i);
      if( !mrb_fixnum_p( shape_val ) ) {
        mrb_raisef( mrb, E_ARGUMENT_ERROR, "invalid shape");
      }
    }
  }
  else {
    mrb_raisef( mrb, E_ARGUMENT_ERROR, "invalid shape");
  }

  return mrb_buffer_alloc(mrb, self, type_no, mrb_shape, shape_len);
}

MRB_API mrb_value
mrb_buffer_init_copy(mrb_state *mrb, mrb_value copy)
{
  mrb_buffer *buffer;
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
    buffer = mrb_malloc(mrb, sizeof(mrb_buffer));
    buffer->shape = NULL;
    buffer->data = NULL;

    DATA_TYPE(copy) = &mrb_buffer_type;
    DATA_PTR(copy) = buffer;

    buffer->type = ((mrb_buffer*)DATA_PTR(src))->type;
    buffer->dim = ((mrb_buffer*)DATA_PTR(src))->dim;

    buffer->shape = mrb_malloc(mrb, sizeof(uint32_t) * buffer->dim);
    
    src_shape_ptr = ((mrb_buffer*)DATA_PTR(src))->shape;
    dst_shape_ptr = buffer->shape;
    for( i = 0; i < buffer->dim; i++ ) {
      *dst_shape_ptr++ = *src_shape_ptr++;
    }

    buffer->size = ((mrb_buffer*)DATA_PTR(src))->size;
    buffer->data = mrb_malloc(mrb, buffer->size * buffer_type_size[buffer->type]);

    src_data_ptr = ((mrb_buffer *)DATA_PTR(src))->data;
    dst_data_ptr = buffer->data;

    for( i = 0; i < (buffer->size * buffer_type_size[buffer->type]); i++ ){
      *dst_data_ptr++ = *src_data_ptr++;
    }
  }

  return copy;
}

MRB_API mrb_value
mrb_buffer_get_type_no(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(((mrb_buffer *)DATA_PTR(self))->type);
}

MRB_API mrb_value
mrb_buffer_get_dim(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(((mrb_buffer *)DATA_PTR(self))->dim);
}

MRB_API mrb_value
mrb_buffer_get_size(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(((mrb_buffer *)DATA_PTR(self))->size);
}

MRB_API mrb_value
mrb_buffer_get_shape(mrb_state *mrb, mrb_value self)
{
  mrb_value ary;
  uint32_t dim = ((mrb_buffer *)DATA_PTR(self))->dim;
  uint32_t *shape = ((mrb_buffer *)DATA_PTR(self))->shape;
  uint32_t i;

  ary = mrb_ary_new_capa(mrb, dim);

  for(i = 0; i < dim; i++) {
    mrb_ary_set(mrb, ary, i, mrb_fixnum_value(*shape++));
  }
  return ary;
}

static uint8_t* mrb_buffer_at(mrb_state *mrb, mrb_value self, mrb_value *mrb_elm, uint32_t elm_num)
{
  uint32_t dim, size, type;
  uint32_t *shape;
  uint32_t pos = 0;
  mrb_buffer *buffer= DATA_PTR(self);

  if( elm_num != buffer->dim ){
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid dim number");
  }

  shape = buffer->shape;
  size = buffer->size;

  for( dim = 0; dim < (buffer->dim); dim++ ){
    size /= *(shape++);

    if(mrb_fixnum_p(*mrb_elm)) {
      if( mrb_fixnum(*mrb_elm) < (buffer->shape[dim]) ) {
        pos += mrb_fixnum(*mrb_elm) * size;
      }
      else {
        mrb_raisef(mrb, E_ARGUMENT_ERROR, "dim[%d] size %d is must under %d", dim, mrb_fixnum(*mrb_elm), buffer->shape[dim]);
      }
    }
    else{
      mrb_raise(mrb, E_ARGUMENT_ERROR, "shape must be fixnum");
    }
    mrb_elm++;
  }

  type = ((mrb_buffer *)DATA_PTR(self))->type;

  pos *= buffer_type_size[type];
  return ((mrb_buffer *)DATA_PTR(self))->data + pos;
}

MRB_API mrb_value
mrb_buffer_get_value(mrb_state *mrb, mrb_value self)
{
  mrb_value *mrb_elm;
  uint32_t type,elm_num;
  uint8_t *data;

  mrb_get_args(mrb, "*", &mrb_elm, &elm_num);

  type = ((mrb_buffer *)DATA_PTR(self))->type;
  data = mrb_buffer_at(mrb, self, mrb_elm, elm_num);

  switch(type) {
    case buffer_type_uint8:    return mrb_fixnum_value( *(uint8_t *)data );
    case buffer_type_int8:     return mrb_fixnum_value( *(int8_t *)data );
    case buffer_type_uint16:   return mrb_fixnum_value( *(uint16_t *)data );
    case buffer_type_int16_t:  return mrb_fixnum_value( *(int16_t *)data );
    case buffer_type_uint32_t: return mrb_fixnum_value( *(uint32_t *)data );
    case buffer_type_int32_t:  return mrb_fixnum_value( *(int32_t *)data );
    case buffer_type_uint64_t: return mrb_fixnum_value( *(uint64_t *)data );
    case buffer_type_int64_t:  return mrb_fixnum_value( *(int64_t *)data );
#ifndef MRB_WITHOUT_FLOAT
    case buffer_type_float:    return mrb_float_value( mrb, *(float *)data );
    case buffer_type_double:   return mrb_float_value( mrb, *(double *)data );
#endif
    default:                mrb_raisef(mrb, E_ARGUMENT_ERROR, "unexpected type %d at get", type);
  }
}

#ifndef MRB_WITHOUT_FLOAT
#define mrb_buffer_set_val(data,type,val) do { \
  if( mrb_fixnum_p(val) ) *(type *)data = mrb_fixnum(val); else \
  if( mrb_float_p(val) )  *(type *)data = mrb_float(val);  \
} while(0)
#else
#define mrb_buffer_set_val(data,type,val) do { \
  if( mrb_fixnum_p(val) ) *(type *)data = mrb_fixnum(val); \
} while(0)
#endif

static void mrb_buffer_set_val_each_type(mrb_state *mrb, uint8_t *data, uint32_t type, mrb_value val)
{
  switch(type) {
    case buffer_type_uint8:    mrb_buffer_set_val(data, uint8_t,  val); break;
    case buffer_type_int8:     mrb_buffer_set_val(data, int8_t,   val); break;
    case buffer_type_uint16:   mrb_buffer_set_val(data, uint16_t, val); break;
    case buffer_type_int16_t:  mrb_buffer_set_val(data, int16_t,  val); break;
    case buffer_type_uint32_t: mrb_buffer_set_val(data, uint32_t, val); break;
    case buffer_type_int32_t:  mrb_buffer_set_val(data, int32_t,  val); break;
    case buffer_type_uint64_t: mrb_buffer_set_val(data, uint64_t, val); break;
    case buffer_type_int64_t:  mrb_buffer_set_val(data, int64_t,  val); break;
#ifndef MRB_WITHOUT_FLOAT
    case buffer_type_float:    mrb_buffer_set_val(data, float,    val); break;
    case buffer_type_double:   mrb_buffer_set_val(data, double,   val); break;
#endif
    default:                mrb_raisef(mrb, E_ARGUMENT_ERROR, "unexpected type %d at set", type);
  }
}

MRB_API mrb_value
mrb_buffer_set_value(mrb_state *mrb, mrb_value self)
{
  mrb_value *mrb_elm;
  uint32_t type,elm_num;
  uint8_t *data;

  mrb_get_args(mrb, "*", &mrb_elm, &elm_num);

  type = ((mrb_buffer *)DATA_PTR(self))->type;
  data = mrb_buffer_at(mrb, self, mrb_elm, elm_num-1);
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

  mrb_buffer_set_val_each_type(mrb, data, type, *mrb_elm);

  return *mrb_elm; 
}

MRB_API mrb_value
mrb_buffer_fill_value(mrb_state *mrb, mrb_value self)
{
  mrb_value mrb_val;
  uint32_t type,size,i;
  uint8_t *data;

  mrb_get_args(mrb, "o", &mrb_val);

  data = ((mrb_buffer *)DATA_PTR(self))->data;
  type = ((mrb_buffer *)DATA_PTR(self))->type;
  size = ((mrb_buffer *)DATA_PTR(self))->size;
  
  for(i = 0; i < size; i++){
    mrb_buffer_set_val_each_type(mrb, data, type, mrb_val);
    data += buffer_type_size[type];
  }

  return mrb_val; 
}

void mrb_mruby_buffer_gem_init(mrb_state *mrb)
{
  mrb_int i;
  mrb_int class_num = sizeof(buffer_sub_class_name_list) / sizeof(buffer_sub_class_name_list[0]);

  struct RClass *cls = mrb_define_class(mrb, "Buffer", mrb->object_class);

  mrb_define_class_method(mrb, cls, "support_float?", mrb_buffer_support_float, MRB_ARGS_NONE());

  MRB_SET_INSTANCE_TT(cls, MRB_TT_DATA);

  mrb_define_method(mrb, cls, "initialize", mrb_buffer_initialize, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, cls, "initialize_copy", mrb_buffer_init_copy, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, cls, "dim", mrb_buffer_get_dim, MRB_ARGS_NONE());
  mrb_define_method(mrb, cls, "size", mrb_buffer_get_size, MRB_ARGS_NONE());
  mrb_define_method(mrb, cls, "shape", mrb_buffer_get_shape, MRB_ARGS_NONE());

  mrb_define_method(mrb, cls, "fill", mrb_buffer_fill_value, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, cls, "[]", mrb_buffer_get_value, MRB_ARGS_ANY());
  mrb_define_method(mrb, cls, "[]=", mrb_buffer_set_value, MRB_ARGS_ANY());

  for( i = 0; i < class_num; i++) {
    buffer_sub_class_list[i] = mrb_define_class_under(mrb, cls, buffer_sub_class_name_list[i], cls);
  }
}

void mrb_mruby_buffer_gem_final(mrb_state *mrb)
{
  // nothing to do.
}

