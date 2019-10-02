class CArray
  NONE   = -1
  UINT8  = 0
  INT8   = 1
  UINT16 = 2
  INT16  = 3
  UINT32 = 4
  INT32  = 5
  UINT64 = 6
  INT64  = 7
  FLOAT  = 8
  DOUBLE = 9

  def initialize(type, shape)
    raise ArgumentError, "unknown type" unless (type.is_a? Integer) && type >= UINT8 && type <= DOUBLE
    case shape
    when Integer
      shape = [shape]
    when Array
      shape.each {|a| raise ArgumentError, "invalid shape" unless a.is_a? Integer }
    else
      raise ArgumentError, "invalid shape"
    end
    alloc(type,shape)
  end

  def type
    return CArray.constants.each { |c|
      break c if CArray.const_get(c) == self.type_no
    }
  end

  class Uint8  < CArray; def initialize(shape); super UINT8,  shape; end; end
  class Int8   < CArray; def initialize(shape); super INT8,   shape; end; end
  class Uint16 < CArray; def initialize(shape); super UINT16, shape; end; end
  class Int16  < CArray; def initialize(shape); super INT16,  shape; end; end
  class Uint32 < CArray; def initialize(shape); super UINT32, shape; end; end
  class Int32  < CArray; def initialize(shape); super INT32,  shape; end; end
  class Uint64 < CArray; def initialize(shape); super UINT64, shape; end; end
  class Int64  < CArray; def initialize(shape); super INT64,  shape; end; end
  class Float  < CArray; def initialize(shape); super FLOAT,  shape; end; end
  class Double < CArray; def initialize(shape); super DOUBLE, shape; end; end

  def inspect
    info = "#{self.class.to_s} "
    info += "#{self.type} "if self.class == CArray
    info += shape.to_s
  end
end

