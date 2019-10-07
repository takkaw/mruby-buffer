class CArray
  TYPE = []

  [
    :Uint8,  :Int8,
    :Uint16, :Int16,
    :Uint32, :Int32,
    :Uint64, :Int64,
    :Float,  :Double
  ].each { |klass|
    TYPE << CArray.const_set(klass, Class.new(CArray) { def initialize(shape); super shape; end })
  }

  def initialize(shape)
    raise StandardError, "Cannot create an instance, only can create CArray sub class(CArray::TYPE) instance" unless TYPE.include? self.class
    case shape
    when Integer
      shape = [shape]
    when Array
      shape.each {|a| raise ArgumentError, "invalid shape" unless a.is_a? Integer }
    else
      raise ArgumentError, "invalid shape"
    end
    alloc(CArray::TYPE.find_index(self.class),shape)
  end

  def inspect
    "#{self.class.to_s} #{shape.to_s}"
  end
end

